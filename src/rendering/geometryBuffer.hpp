#pragma once

#include "GL/glew.h"

namespace cgvkp
{
	namespace rendering
	{
		class GeometryBuffer
		{
		public:
			enum TextureType
			{
				positionView = 0,
				normalView,
				diffuseColor,
				material,
				final,
				numTextures
			};

			GeometryBuffer();
			inline ~GeometryBuffer() { deinit(); }
			bool init();
			void deinit();
			void bindForWritingGeometry() const;
			void bindForReadingGeometry() const;
			void bindForWritingFinal() const;
			void bindForReadingFinal() const;
			void resize(GLsizei width, GLsizei height) const;

		private:
			GLuint framebuffer;
			GLuint textures[numTextures];
			GLuint depthStencilTexture;
		};
	}
}
