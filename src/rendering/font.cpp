#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif
#include "font.hpp"
#include "util/bezier.hpp"
#include <fstream>
#include "poly2tri/poly2tri/poly2tri.h"

cgvkp::rendering::Font::Font()
	: library(nullptr), face(nullptr)
{
}

cgvkp::rendering::Font::~Font()
{
	deinit();
}

bool cgvkp::rendering::Font::init(char const* filename, FT_ULong startCodePoint, FT_ULong lastCodePoint)
{
	// Initialize library.
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not initialize FreeType library (error code: " << error << ")." << std::endl;
#endif
		return false;
	}

	// Load face.
	error = FT_New_Face(library, filename, 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << '"' << filename << "\" has an unsupported file format." << std::endl;
#endif
		return false;
	}
	else if (error)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "An error occured loading \"" << filename << "\" (error code: " << error << ")." << std::endl;
#endif
		return false;
	}

	for (; startCodePoint <= lastCodePoint; ++startCodePoint)
	{
		loadGlyph(startCodePoint);
	}

	return true;
}

void cgvkp::rendering::Font::deinit()
{
	for (auto const& g : glyphs)
	{
		glDeleteVertexArrays(1, &g.second.vertexArray);
		glDeleteBuffers(1, &g.second.vertexBuffer);
		glDeleteBuffers(1, &g.second.indexBuffer);
	}
	glyphs.clear();

	if (face)
	{
		FT_Done_Face(face);
		face = nullptr;
	}
	if (library)
	{
		FT_Done_FreeType(library);
		library = nullptr;
	}
}

float cgvkp::rendering::Font::getWidth(std::string const& str) const
{
	float width = 0;
	for(auto c : str)
	{
		auto it = glyphs.find(c);
		if (it != glyphs.end())
		{
			width += it->second.width;
		}
	}

	return width;
}

float cgvkp::rendering::Font::render(FT_ULong codePoint) const
{
	auto it = glyphs.find(codePoint);
	if (it != glyphs.end())
	{
		glBindVertexArray(it->second.vertexArray);
		glDrawElements(GL_TRIANGLES, it->second.numIndices, GL_UNSIGNED_INT, nullptr);
		return it->second.width;
	}

	return 0;
}

bool cgvkp::rendering::Font::loadGlyph(FT_ULong codePoint)
{
	FT_UInt glyphIndex = FT_Get_Char_Index(face, codePoint);

	FT_Error error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_NO_SCALE | FT_LOAD_LINEAR_DESIGN);
	if (error)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not load glyph for char code ";
		if (codePoint >= 0x20 && codePoint < 0x7F)	// Printable ASCII
		{
			std::cerr << '\'' << static_cast<char>(codePoint) << '\'';
		}
		else
		{
			std::cerr << codePoint;
		}
		std::cerr << " (error code: " << error << ")." << std::endl;
#endif
		return false;
	}


	FT_Outline& outline = face->glyph->outline;
	FT_Pos deltaX = face->glyph->metrics.horiBearingX;
	FT_Pos deltaY = -face->bbox.yMin;
	float scale = 1.0f / face->units_per_EM;


	// Contours are closed paths.
	// Outside contours of an outline are oriented in clock-wise direction, as defined in the TrueType specification.
	int iPoint = 0;
	for (int iCountour = 0; iCountour < outline.n_contours; ++iCountour)
	{
		conic = false;
		vertexRange vr;
		vr.contour = iCountour;
		vr.start = static_cast<int>(vertices.size());
		vr.num = 0;
		int firstPoint = iPoint;

		std::vector<glm::vec2> curvePoints;
		for (; iPoint <= outline.contours[iCountour]; ++iPoint)
		{
			//calculate Points
			FT_Vector& point = outline.points[iPoint];
			float x = (point.x + deltaX) * scale;
			float y = (point.y + deltaY) * scale;

			if (outline.tags[iPoint] & 0x01)
			{
				// Point on curve.
				curvePoints.push_back(glm::vec2(x, y));
				if(curvePoints.size() > 1)
				{ 
					addCurve(curvePoints, vr, conic);
					glm::vec2 tmp = curvePoints.back();
					curvePoints.clear();
					conic = false;
					curvePoints.push_back(tmp);
				}
			}
			else
			{
				// Point off curve.
				curvePoints.push_back(glm::vec2(x, y));
				conic = !(outline.tags[iPoint] & 0x02);
			}
		}

		//add last curve vec
		FT_Vector& point = outline.points[firstPoint];
		float x = (point.x + deltaX) * scale;
		float y = (point.y + deltaY) * scale;
		curvePoints.push_back(glm::vec2(x, y));
		addCurve(curvePoints, vr, conic);

		getClockwise(vr.clockwise, outline.points, vr);

		contours.push_back(vr);
	}
	
	std::vector<glm::vec3> vertices;
	std::vector<int> indices;
	trinagulate(vertices, indices);

	Glyph glyph;
	glyph.width = static_cast<float>(face->glyph->linearHoriAdvance * scale);
	
	glGenVertexArrays(1, &glyph.vertexArray);
	glBindVertexArray(glyph.vertexArray);
	
	glGenBuffers(1, &glyph.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, glyph.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &glyph.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyph.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glyph.numIndices = static_cast<GLsizei>(indices.size());

	glyphs.insert({ codePoint, glyph });

	return true;
}

void cgvkp::rendering::Font::addCurve(std::vector<glm::vec2> const& contourVertices, vertexRange& vr, bool concic)
{
	if (contourVertices.size() == 2)
	{	
		vertices.push_back(contourVertices[0]);
		vr.num += 1;
	}
	else 
	{
		util::Bezier<glm::vec2> bezier(concic, contourVertices);
		float samplingRate = bezier.getLength() * 40.0f; // length * density in one unit
		
		for (int i = 0; i < samplingRate; ++i)
		{
			vertices.push_back(bezier.sample(static_cast<float>(i) / samplingRate));
			vr.num++;
		}
	}
}


void cgvkp::rendering::Font::getClockwise(bool& clockwise, FT_Vector* points, vertexRange& vr)
{
	int j, k, h;
	int count = 0;
	double z;

	for (int i = 0; i < (vr.num / 1.5); i++) 
	{
		h = (i % vr.num) + vr.start;
		j = ((i + 1) % vr.num) + vr.start;
		k = ((i + 2) % vr.num) + vr.start;

		z = (points[j].x - points[h].x) * (points[k].y - points[j].y);
		z -= (points[j].y - points[h].y) * (points[k].x - points[j].x);
		if (z < 0)
			count--;
		else if (z > 0)
			count++;
	}
	if (count >= 0)
		clockwise = false;
	else if (count < 0)
		clockwise = true;
}

void cgvkp::rendering::Font::save(char * filename)
{
	std::ofstream output(filename);
	for (int i = 0; i < vertices.size(); i++)
	{
		output << "v " << vertices[i].x << " " << vertices[i].y << " " << "1.0" << std::endl;
	}

	for (int i = 0; i < indices.size(); i = i + 3)
	{
		output << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << std::endl;
	}
	output.close();
}

void cgvkp::rendering::Font::trinagulate(std::vector<glm::vec3>& glyphVert, std::vector<int>& glyphInd)
{
	std::vector<p2t::Triangle*> triangles;
	std::vector<p2t::Point*> polyline;
	std::vector<std::vector<p2t::Point*>> polylines;
	std::map<p2t::Point*, int> points;
	p2t::CDT* cdt = new p2t::CDT(polyline);


	for (int i = 0; i < contours.size(); i++)
	{
		//outlines ---------------------------------------
		if (contours[i].clockwise)
		{
			for (int j = contours[i].start; j < contours[i].start + contours[i].num; j++)
			{
				p2t::Point* point = new p2t::Point(vertices[j].x, vertices[j].y);
				polyline.push_back(point);
				points[point] = j;
			}
			polylines.push_back(polyline);
			cdt = new p2t::CDT(polyline);
			polyline.clear();
		}
		//inlines -----------------------------------------
		if (!contours[i].clockwise)
		{
			for (int j = contours[i].start; j < contours[i].start + contours[i].num; j++)
			{
				p2t::Point* point = new p2t::Point();
				point->set(vertices[j].x, vertices[j].y);
				polyline.push_back(point);
				points[point] = j;
			}
			cdt->AddHole(polyline);
			polylines.push_back(polyline);
			polyline.clear();
		}

		// add indices-------------------------------------
		if (i == contours.size() -1 || contours[i + 1].clockwise)
		{
			// triangulate---------------------------------
			cdt->Triangulate();
			triangles = cdt->GetTriangles();

			// generate Indices----------------------------
			for (int i = 0; i < triangles.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					p2t::Point* point1 = triangles[i]->GetPoint(j);
					indices.push_back(points.find(point1)->second);
				}
			}
			for (auto p : points)
			{
				delete p.first;
			}
			delete cdt;
			polylines.clear();
			polyline.clear();
			points.clear();
		}
	}
	save("output.obj");
	ConvertTo3D(glyphVert, glyphInd);
}


void cgvkp::rendering::Font::ConvertTo3D(std::vector<glm::vec3>& glyphVert, std::vector<int>& glyphInd)
{
	// add front vertices 

	int vs = static_cast<int>(vertices.size());

	for (int i = 0; i < vs; i++)
	{
		glyphVert.emplace_back(vertices[i].x, vertices[i].y, 0);
	}

	// add front indices
	glyphInd.insert(glyphInd.end(), indices.begin(), indices.end());
	
	// add back vertices
	for (int i = 0; i < vs; i++)
	{
		glyphVert.emplace_back(vertices[i].x, vertices[i].y, -0.2);
	}

	// add back indices
	for (int i = 0; i < indices.size(); i = i++)
	{
		glyphInd.push_back(indices[i] + static_cast<int>(vs));
	}

	// bind side vertices
	for (int j = 0; j < contours.size(); j++)
	{
		for (int i = 0; i < contours[j].num ; ++i)
		{
			int k = i + contours[j].start;
			int l = ((i + 1) % contours[j].num) + contours[j].start;

			glyphInd.push_back(k);
			glyphInd.push_back(l);
			glyphInd.push_back(k + vs);
			
			glyphInd.push_back(k + vs);
			glyphInd.push_back(l);
			glyphInd.push_back(l + vs);
		}
	}
	contours.clear();
	vertices.clear();
	indices.clear();
}