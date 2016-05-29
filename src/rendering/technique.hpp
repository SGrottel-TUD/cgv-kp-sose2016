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
			virtual ~Technique();
			virtual bool init();
			void deinit();
			inline void use() const { glUseProgram(program); }

		protected:
			Technique();
			bool addShader(GLenum shaderType, char const* filename);
			GLint getUniformLocation(GLchar const* name) const;
			bool link();

		private:
			GLuint program;
			std::list<GLuint> shaders;
		};
	}
}
