#pragma once

//#include <GL/glew.h>
#include <map>
#include <string>
#include <ft2build.h>
#include <glm/glm.hpp>
#include <vector>
#include FT_FREETYPE_H
#include "poly2tri/poly2tri/poly2tri.h"
#include "GL\glew.h"


namespace cgvkp
{
	namespace rendering
	{
		struct Glyph
		{
		public:
			float width;
			GLuint vertexArray;
			GLuint vertexBuffer;
			GLuint indexBuffer;
			GLsizei numIndices;
		};

		class Font
		{
		public:
			struct vertexRange
			{
				int start;
				int num;
				int contour;
				bool clockwise;
			};

			Font();
			~Font();
			bool init(char const* filename, FT_ULong startCodePoint, FT_ULong lastCodePoint);
			void deinit();
			inline float getHeight() const { return static_cast<float>(face->height) / face->units_per_EM; }
			float getWidth(std::string const& str) const;
			float render(FT_ULong codePoint) const;
			void addCurve(std::vector<glm::vec2> const& contourVertices,vertexRange& vr, bool concic);
			void cgvkp::rendering::Font::getClockwise(bool& clockwise, FT_Vector* points, vertexRange& vr);
			void save(char * filename);
			void trinagulate(std::vector<glm::vec3>& GlyphVert, std::vector<int>& GlyphInd);
			void ConvertTo3D(std::vector<glm::vec3>& GlyphVert, std::vector<int>& GlyphInd);


		private:
			bool loadGlyph(FT_ULong codePoint);

			FT_Library library;
			FT_Face face;
			std::vector<vertexRange> contours;
			bool conic;		
			std::map<FT_ULong, Glyph> glyphs;
			std::vector<glm::vec2> vertices;
			std::vector<int> indices;
		};
	}
}
