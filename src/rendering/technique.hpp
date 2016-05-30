#pragma once

#include "glm/mat4x4.hpp"
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
            virtual void setProjectionView(glm::mat4x4 const& projectionView) const;

		protected:
			Technique();
			bool addShader(GLenum shaderType, char const* filename);
			GLint getUniformLocation(GLchar const* name) const;
			bool link();
            GLint projectionViewLocation;

		private:
			GLuint program;
			std::list<GLuint> shaders;
		};
	}
}
