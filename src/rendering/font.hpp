#pragma once

#include <map>
#include <string>
#include <ft2build.h>
#include <glm/glm.hpp>
#include <vector>
#include FT_FREETYPE_H
#include <GL/glew.h>


namespace cgvkp
{
	namespace rendering
	{
		class Font
		{
		public:
			Font(char const* pFilename);
			~Font();
			bool init();
			void deinit();
			inline glm::vec2 getSize(char const* str, float fontSize) const { return glm::vec2(getWidth(str, fontSize), getHeight(fontSize)); }
			inline glm::vec2 getSize(FT_ULong codePoint, float fontSize) const { return glm::vec2(getWidth(codePoint, fontSize), getHeight(fontSize)); }
			float getWidth(FT_ULong codePoint, float fontSize) const;
			float getWidth(char const* str, float fontSize) const;
			inline float getWidth(std::string const& str, float fontSize) const { return getWidth(str.c_str(), fontSize); }
			inline float getHeight(float fontSize) const { return (fontSize * face->height) / face->units_per_EM; }
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
			struct Glyph
			{
			public:
				float width;
				GLuint vertexArray;
				GLuint vertexBuffer;
				GLuint indexBuffer;
				GLsizei numIndices;

				std::vector<glm::vec3> vertices;
				std::vector<int> indices;
			};

			bool loadGlyph(FT_ULong codePoint);
			void addCurve(std::vector<glm::vec2> const& contourVertices, Contour& vr, bool conic, std::vector<glm::vec2>& vertices) const;
			bool getClockwise(std::vector<glm::vec2> const& onPoints) const;
			void triangulate(std::vector<Contour> const& contours, std::vector<glm::vec2>& vertices, std::vector<int>& indices) const;
			void convertTo3D(Glyph& glyph, std::vector<Contour> const& contours, std::vector<glm::vec2> const& vertices, std::vector<int> const& indices) const;

			FT_Library library;
			FT_Face face;
			std::map<FT_ULong, Glyph> glyphs;
		};
	}
}
