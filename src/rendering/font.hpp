#pragma once

#include <ft2build.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>
#include FT_FREETYPE_H

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
			bool loadGlyph(FT_ULong codePoint);
			inline glm::vec2 getSize(char const* str, float fontSize) const { return glm::vec2(getWidth(str, fontSize), getHeight(fontSize)); }
			inline glm::vec2 getSize(FT_ULong codePoint, float fontSize) const { return glm::vec2(getWidth(codePoint, fontSize), getHeight(fontSize)); }
			float getWidth(FT_ULong codePoint, float fontSize) const;
			float getWidth(char const* str, float fontSize) const;
			inline float getWidth(std::string const& str, float fontSize) const { return getWidth(str.c_str(), fontSize); }
			inline float getHeight(float fontSize) const { return (fontSize * face->height) / face->units_per_EM; }
			float render(FT_ULong codePoint) const;

		private:
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

			FT_Library library;
			FT_Face face;
			std::map<FT_ULong, Glyph> glyphs;
		};
	}
}
