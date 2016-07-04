#pragma once

#include <GL/glew.h>

namespace cgvkp
{
	namespace rendering
	{
		class PostProcessingFramebuffer
		{
		public:
			PostProcessingFramebuffer() : framebuffer(0), numTextures(0), pTextures(nullptr) {}
			inline ~PostProcessingFramebuffer() { deinit(); }
			bool init(GLsizei numNeededTextures);
			void deinit();
			void resize(GLsizei width, GLsizei height) const;
			void nextPass(int nextDrawBuffer, int numTexturesToBind = 1) const;
			void finalPass(int currentDrawBuffer, int numTexturesToBind = 1) const;

		private:
			GLuint framebuffer;
			GLsizei numTextures;
			GLuint* pTextures;
		};
	}
}
