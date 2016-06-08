#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
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
			void setWorldViewProjection(glm::mat4x4 const& worldViewProjection) const;
			inline void use() const { glUseProgram(program); }

		protected:
			Technique();
			bool addShader(GLenum shaderType, char const* filename);
			GLint getUniformLocation(GLchar const* name) const;
			bool link();

			GLint worldViewProjectionLocation;

		private:
			GLuint program;
			std::list<GLuint> shaders;
		};
	}
}
