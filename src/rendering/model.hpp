#pragma once
#include "GL/glew.h"

namespace cgvkp {
	namespace rendering {
		// model for stars, hands and clouds
		struct model {
			GLuint vertex_buffer;
			GLuint index_buffer;
			GLuint texture;
			// ...
		};
	}
}
