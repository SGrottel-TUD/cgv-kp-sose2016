#pragma once

#include <GL/glew.h>
#include <list>

namespace cgvkp
{
	namespace rendering
	{
		class Technique
		{
		public:
			virtual inline ~Technique() { deinit(); }
			virtual void deinit();
			inline void use() const { glUseProgram(program); }

		protected:
			inline Technique() : program(0) {}
			bool addShader(GLenum shaderType, char const* pFilename);
			GLint getUniformLocation(GLchar const* pName) const;
			bool init();
			bool link();

			static GLint const invalidLocation = -1;

		private:
			GLuint program;
			std::list<GLuint> shaders;
		};
	}
}
