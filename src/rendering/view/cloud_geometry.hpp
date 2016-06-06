#pragma once
#include "rendering/view/view_geometry_base.hpp"
#include "GL/glew.h"
#include <glm/glm.hpp>

namespace cgvkp {
	namespace rendering {
		namespace view {
			class cloud_geometry {
			public:
				~cloud_geometry();
				cloud_geometry();
				bool init();
				GLuint vao;
				GLuint buffers[2];
				GLuint element_count;
			};
		}
	}
}
