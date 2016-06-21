#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif
#include "font.hpp"
#include <math.h>
#include "util/bezier.hpp"
#include <fstream>
#include <ftoutln.h>
#include "util/resource_file.hpp"
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
	error = FT_New_Face(library, util::resource_file::find_resource_file(filename).c_str(), 0, &face);
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

float cgvkp::rendering::Font::getWidth(char const* str, float fontSize) const
{
	float width = 0;
	for (; *str; ++str)
	{
		auto it = glyphs.find(*str);
		if (it != glyphs.end())
		{
			width += it->second.width;
		}
	}

	return width * fontSize;
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
		Contour contour;
		contour.start = static_cast<int>(vertices.size());
		contour.num = 0;
		contour.min.x = contour.min.y = 1;
		contour.max.x = contour.max.y = 0;

		std::vector<glm::vec2> onPoints;
		std::vector<glm::vec2> curvePoints;
		bool conic = false;

		glm::vec2 firstPoint((outline.points[iPoint].x + deltaX) * scale, (outline.points[iPoint].y + deltaY) * scale);

		curvePoints.push_back(firstPoint);
		++iPoint;

		for (; iPoint <= outline.contours[iCountour]; ++iPoint)
		{
			//calculate Points
			FT_Vector& p = outline.points[iPoint];
			glm::vec2 point((p.x + deltaX) * scale, (p.y + deltaY) * scale);

			if (outline.tags[iPoint] & FT_CURVE_TAG_ON)
			{
				onPoints.push_back(point);
				curvePoints.push_back(point);
				addCurve(curvePoints, contour, conic);
				curvePoints.clear();
				conic = false;
				curvePoints.push_back(point);
			}
			else
			{
				curvePoints.push_back(point);
				conic = !(outline.tags[iPoint] & FT_CURVE_TAG_CUBIC);
			}
		}

		//add last curve vec
		curvePoints.push_back(firstPoint);
		addCurve(curvePoints, contour, conic);

		contour.clockwise = getClockwise(onPoints);
		contours.push_back(contour);
	}
	
	std::vector<glm::vec3> vertices;
	std::vector<int> indices;
	triangulate(vertices, indices);

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

void cgvkp::rendering::Font::addCurve(std::vector<glm::vec2> const& contourVertices, Contour& contour, bool conic)
{
	if (contourVertices.size() == 2)
	{	
		vertices.push_back(contourVertices.front());
		contour.num += 1;
	}
	else 
	{
		util::Bezier<glm::vec2> bezier(conic ? 2 : 3, contourVertices);
		int samplingRate = static_cast<int>(bezier.getLength() * 40); // length * density in one unit
		
		for (int i = 0; i < samplingRate; ++i)
		{
			glm::vec2 point = bezier.sample(static_cast<float>(i) / samplingRate);

			vertices.push_back(point);

			// Update aabb.
			if (point.x < contour.min.x)
			{
				contour.min.x = point.x;
			}
			if (point.y < contour.min.y)
			{
				contour.min.y = point.y;
			}
			if (point.x > contour.max.x)
			{
				contour.max.x = point.x;
			}
			if (point.y > contour.max.y)
			{
				contour.max.y = point.y;
			}
		}
		contour.num += samplingRate;
	}
}

bool cgvkp::rendering::Font::getClockwise(std::vector<glm::vec2> const& onPoints) const
{
	int count = 0;
	for (int i = 0; i < onPoints.size(); i++)
	{
		int j = (i + 1) % onPoints.size();
		int k = (i + 2) % onPoints.size();

		float z = static_cast<float>((onPoints[j].x - onPoints[i].x) * (onPoints[k].y - onPoints[i].y) - (onPoints[j].y - onPoints[i].y) * (onPoints[k].x - onPoints[i].x));
		if (z < 0)
			count--;
		else if (z > 0)
			count++;
	}

#if defined(_DEBUG) || defined(DEBUG)
	if (count == 0)
	{
		std::cerr << "Could not determine whether a contour is clockwise or counter-clockwise." << std::endl;
	}
#endif
	
	return count < 0;
}

void cgvkp::rendering::Font::save(char const* filename)
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

void cgvkp::rendering::Font::triangulate(std::vector<glm::vec3>& glyphVert, std::vector<int>& glyphInd)
{
	struct ContourNode
	{
		Contour* contour;
		std::list<ContourNode> children;
	};
	ContourNode root;

	for (auto& c : contours)
	{

	}

	std::vector<p2t::Triangle*> triangles;
	std::vector<p2t::Point*> polyline;
	std::vector<std::vector<p2t::Point*>> polylines;
	std::map<p2t::Point*, int> points;
	p2t::CDT* cdt = nullptr;


	for (int i = 0; i < contours.size(); i++)
	{
		//std::cout << ' ' << (contours[i].clockwise ? "c" : "cc");
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
		else
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
	//save("output.obj");
	ConvertTo3D(glyphVert, glyphInd);

}


void cgvkp::rendering::Font::ConvertTo3D(std::vector<glm::vec3>& glyphVert, std::vector<int>& glyphInd)
{
	int vs = static_cast<int>(vertices.size());

	// add front vertices, indices
	for (auto const& v : vertices)
	{
		glyphVert.emplace_back(v.x, v.y, 0);
	}
	glyphInd.insert(glyphInd.end(), indices.begin(), indices.end());
	
	// add back vertices
	for (auto const& v : vertices)
	{
		glyphVert.emplace_back(v.x, v.y, -0.2);
	}

	// add back indices
	/*for (int i = 0; i < indices.size(); i = i++)
	{
		glyphInd.push_back(indices[i] + static_cast<int>(vs));
	}*/

	// bind side vertices
	for (auto const& contour : contours)
	{
		for (int i = 0; i < contour.num; ++i)
		{
			int k = i + contour.start;
			int l = ((i + 1) % contour.num) + contour.start;

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