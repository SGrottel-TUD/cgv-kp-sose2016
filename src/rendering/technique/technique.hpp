#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <list>
#include <string>
#include "rendering/view/view_base.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class Technique
		{
		public:
			virtual ~Technique();
			virtual void deinit();
			void setWorldViewProjection(glm::mat4x4 const& worldViewProjection) const;
			inline void use() const { glUseProgram(program); }

		protected:
			Technique();
			bool addShader(GLenum shaderType, std::string const& filename);
			GLint getUniformLocation(GLchar const* name) const;
			bool init();
			bool link();

			static GLint const invalidLocation = -1;
			GLint worldViewProjectionLocation;

		private:
			GLuint program;
			std::list<GLuint> shaders;
		};
	}
}
