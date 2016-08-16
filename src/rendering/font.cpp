#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif
#include "font.hpp"
#include "poly2tri/poly2tri/poly2tri.h"
#include "util/bezier.hpp"
#include "util/resource_file.hpp"

struct Contour
{
	std::vector<p2t::Point> points;

	int start;
	int num;
	glm::vec2 min;
	glm::vec2 max;

	bool contains(Contour const& contour) const;
};

// The possible child contour must have at least one point.
bool Contour::contains(Contour const& contour) const
{
	// To speed up a little, check the aabb first
	if (min.x > contour.min.x || max.x < contour.max.x || min.y > contour.min.y || max.y < contour.max.y)
	{
		return false;
	}

	int count = 0;
	auto const& p = contour.points.front();

	size_t n = points.size();
	for (size_t i = 0; i < n; ++i)
	{
		if (points[i].y <= p.y)
		{
			if (points[(i + 1) % n].y > p.y)
			{
				if ((points[(i + 1) % n].x - points[i].x) * (p.y - points[i].y) - (p.x - points[i].x) * (points[(i + 1) % n].y - points[i].y) > 0)	// point is left of edge
				{
					++count;
				}
			}
		}
		else if (points[(i + 1) % n].y <= p.y)
		{
			if ((points[(i + 1) % n].x - points[i].x) * (p.y - points[i].y) - (p.x - points[i].x) * (points[(i + 1) % n].y - points[i].y) < 0)	// point is right of edge
			{
				--count;
			}
		}
	}

	return count != 0;
}

struct ContourNode
{
	Contour* pContour;
	std::list<ContourNode> children;

	void insert(Contour& contour);
};

void ContourNode::insert(Contour& contour)
{
	for (auto& child : children)
	{
		if (child.pContour->contains(contour))	// contour is grandchild.
		{
			child.insert(contour);
			return;
		}
		else if (contour.contains(*child.pContour))	// contour is child but parent of already inserted children
		{
			std::list<ContourNode> oldChildren;
			oldChildren.swap(children);

			ContourNode node;
			node.pContour = &contour;
			children.push_back(node);

			for (auto const& c : oldChildren)
			{
				if (contour.contains(*c.pContour))
				{
					node.children.push_back(c);
				}
				else
				{
					children.push_back(c);
				}
			}
			return;
		}
	}

	// The contour is no grandchild. Insert it as a child.
	ContourNode node;
	node.pContour = &contour;
	children.push_back(node);
}

void addCurve(std::vector<glm::vec2> const& curvePoints, Contour& contour, bool conic)
{
	if (curvePoints.size() == 2)
	{
		auto& point = curvePoints.front();
		contour.points.push_back(p2t::Point(point.x, point.y));
		contour.num += 1;

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
	else
	{
		cgvkp::util::Bezier<glm::vec2> bezier(conic ? 2 : 3, curvePoints);
		int samplingRate = static_cast<int>(bezier.getLength() * 40); // length * density in one unit

		for (int i = 0; i < samplingRate; ++i)
		{
			glm::vec2 point = bezier.sample(static_cast<float>(i) / samplingRate);

			contour.points.push_back(p2t::Point(point.x, point.y));

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

void insertIntoCDT(p2t::CDT** ppCDT, ContourNode const& node, std::map<p2t::Point*, int>& points)
{
	std::vector<p2t::Point*> polyline;
	for (int i = 0; i < node.pContour->num; i++)
	{
		p2t::Point* point = &node.pContour->points[i];
		polyline.push_back(point);
		points[point] = node.pContour->start + i;
	}

	if (!(*ppCDT))
	{
		*ppCDT = new p2t::CDT(polyline);
	}
	else
	{
		(*ppCDT)->AddHole(polyline);
	}

	for (auto& child : node.children)
	{
		insertIntoCDT(ppCDT, child, points);
	}
}

void triangulate(std::vector<Contour>& contours, std::vector<int>& indices)
{
	ContourNode root;
	for (auto& contour : contours)
	{
		root.insert(contour);
	}

	for (auto const& node : root.children)
	{
		std::map<p2t::Point*, int> points;
		p2t::CDT* pCDT = nullptr;

		insertIntoCDT(&pCDT, node, points);

		pCDT->Triangulate();
		std::vector<p2t::Triangle*> triangles = pCDT->GetTriangles();
		for (p2t::Triangle* pTriangle : triangles)
		{
			for (int i = 0; i < 3; ++i)
			{
				indices.push_back(points.find(pTriangle->GetPoint(i))->second);
			}
		}

		delete pCDT;
	}
}

void convertTo3D(std::vector<Contour> const& contours, std::vector<glm::vec2> const& sourceVertices, std::vector<int> const& sourceIndices, std::vector<glm::vec3>& destVertices, std::vector<int>& destIndices)
{
	int vs = static_cast<int>(sourceVertices.size());

	// Add front vertices, indices.
	for (auto const& v : sourceVertices)
	{
		destVertices.emplace_back(v, 0);
	}
	destIndices.insert(destIndices.end(), sourceIndices.begin(), sourceIndices.end());

	// Add back vertices.
	for (auto const& v : sourceVertices)
	{
		destVertices.emplace_back(v, -0.4f);
	}

	// Connect vertices on the side.
	for (auto const& contour : contours)
	{
		for (int i = 0; i < contour.num; ++i)
		{
			int k = i + contour.start;
			int l = ((i + 1) % contour.num) + contour.start;

			destIndices.push_back(k);
			destIndices.push_back(l);
			destIndices.push_back(k + vs);

			destIndices.push_back(k + vs);
			destIndices.push_back(l);
			destIndices.push_back(l + vs);
		}
	}
}


cgvkp::rendering::Font::Font(char const* pFilename)
	: library(nullptr), face(nullptr)
{
	// Initialize library.
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not initialize FreeType library (error code: " << error << ")." << std::endl;
#endif
		return;
	}

	// Load face.
	error = FT_New_Face(library, util::resource_file::getResourcePath("fonts", pFilename).c_str(), 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << '"' << pFilename << "\" has an unsupported file format." << std::endl;
#endif
		return;
	}
	else if (error)
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "An error occured loading \"" << pFilename << "\" (error code: " << error << ")." << std::endl;
#endif
	}

	for (FT_ULong codePoint = ' '; codePoint <= 256; ++codePoint)
	{
		loadGlyph(codePoint);
	}
}

cgvkp::rendering::Font::~Font()
{
	deinit();

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

bool cgvkp::rendering::Font::init()
{
	if (!library)
	{
		return false;
	}

	for(auto& pair : glyphs)
	{
		glGenVertexArrays(1, &pair.second.vertexArray);
		glBindVertexArray(pair.second.vertexArray);

		glGenBuffers(1, &pair.second.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, pair.second.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, pair.second.vertices.size() * sizeof(glm::vec3), pair.second.vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glGenBuffers(1, &pair.second.indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pair.second.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pair.second.indices.size() * sizeof(int), pair.second.indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		pair.second.numIndices = static_cast<GLsizei>(pair.second.indices.size());
	}

	return true;
}

void cgvkp::rendering::Font::deinit()
{
	for (auto& g : glyphs)
	{
		glDeleteVertexArrays(1, &g.second.vertexArray);
		g.second.vertexArray = GL_NONE;
		glDeleteBuffers(1, &g.second.vertexBuffer);
		g.second.vertexBuffer = GL_NONE;
		glDeleteBuffers(1, &g.second.indexBuffer);
		g.second.indexBuffer = GL_NONE;
	}
}

float cgvkp::rendering::Font::getWidth(FT_ULong codePoint, float fontSize) const
{
	auto it = glyphs.find(codePoint);
	if (it != glyphs.end())
	{
		return it->second.width * fontSize;
	}
	
	return 0;
}

float cgvkp::rendering::Font::getWidth(char const* str, float fontSize) const
{
	float width = 0;
	for (; *str; ++str)
	{
		width += getWidth(static_cast<unsigned char>(*str), fontSize);
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
	if (glyphs.find(codePoint) != glyphs.end())
	{
		return true;
	}

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

	Glyph& glyph = glyphs.insert({ codePoint, Glyph() }).first->second;
	glyph.width = face->glyph->linearHoriAdvance * scale;


	// Contours are closed paths.
	// Outside contours of an outline are oriented in clock-wise direction, as defined in the TrueType specification.
	std::vector<Contour> contours;
	std::vector<glm::vec2> vertices;
	int iPoint = 0;
	for (int iCountour = 0; iCountour < outline.n_contours; ++iCountour)
	{
		Contour contour;
		contour.start = static_cast<int>(vertices.size());
		contour.num = 0;
		contour.min.x = contour.min.y = 10;
		contour.max.x = contour.max.y = -10;

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
		for (auto const& v : contour.points)
		{
			vertices.push_back(glm::vec2(v.x, v.y));
		}

		contours.push_back(contour);
	}
	std::vector<int> indices;
	triangulate(contours, indices);
	convertTo3D(contours, vertices, indices, glyph.vertices, glyph.indices);

	return true;
}
