#pragma once
#include "rendering/view/view_geometry_base.hpp"
#include "util/texture.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace cgvkp {
	namespace rendering {
		namespace view {
			class cloud_geometry {
			public:
				~cloud_geometry();
				cloud_geometry();
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
