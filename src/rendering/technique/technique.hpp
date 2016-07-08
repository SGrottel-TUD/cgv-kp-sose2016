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
			GLint getUniformLocation(GLchar const* pName) const;
			bool init(char const* vertexShader, char const* geometryShader, char const* fragmentShader);
			inline bool init(char const* vertexShader, char const* fragmentShader) { return init(vertexShader, nullptr, fragmentShader); }

			static GLint const invalidLocation = -1;

		private:
			bool addShader(GLenum shaderType, char const* pFilename);
			bool link();

			GLuint program;
			std::list<GLuint> shaders;
		};
	}
}
