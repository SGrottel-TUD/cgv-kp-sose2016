#pragma once
#include "rendering/view/view_geometry_base.hpp"
#include "util/texture.hpp"
#include <GL/glew.h>

namespace cgvkp {
namespace rendering {
namespace view {
	class star_geometry {
	public:
		~star_geometry();
		star_geometry();
		bool init();
        std::shared_ptr<util::texture> texture;
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