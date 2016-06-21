#pragma once

#include <map>
#include <string>
#include <ft2build.h>
#include "glm/glm.hpp"
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
			Font();
			~Font();
			bool init(char const* filename, FT_ULong startCodePoint, FT_ULong lastCodePoint);
			void deinit();
			inline float getHeight(float fontSize) const { return (fontSize * face->height) / face->units_per_EM; }
			inline float getWidth(std::string const& str, float fontSize) const { return getWidth(str.c_str(), fontSize); }
			float getWidth(char const* str, float fontSize) const;
			float render(FT_ULong codePoint) const;

		private:
			struct Contour
			{
				int start;
				int num;
				bool clockwise;
				glm::vec2 min;
				glm::vec2 max;
			};

			bool loadGlyph(FT_ULong codePoint);
			void addCurve(std::vector<glm::vec2> const& contourVertices, Contour& vr, bool conic);
			bool getClockwise(std::vector<glm::vec2> const& onPoints) const;
			void save(char const* filename);
			void triangulate(std::vector<glm::vec3>& GlyphVert, std::vector<int>& GlyphInd);
			void ConvertTo3D(std::vector<glm::vec3>& GlyphVert, std::vector<int>& GlyphInd);

			FT_Library library;
			FT_Face face;
			std::vector<Contour> contours;	
			std::map<FT_ULong, Glyph> glyphs;
			std::vector<glm::vec2> vertices;
			std::vector<int> indices;
		};
	}
}
