#pragma once
#include "rendering/view/view_geometry_base.hpp"
#include "GL/glew.h"

namespace cgvkp {
namespace rendering {
namespace view {
	class hand_geometry {
	public:
		~hand_geometry();
		hand_geometry();
		bool init();
		GLuint vao;
		GLuint vertexBuffers[numAttributes];
		GLuint indexBuffer;
		GLenum indicesMode;
		GLsizei indicesCount;
		GLenum indicesType;
	};
}
}
}
