#pragma once
#include "mono_camera.hpp"
#include "GL/glew.h"

namespace cgvkp {
	namespace rendering {
		class light : public mono_camera {
		public:
			explicit light(abstract_renderer* renderer);
			virtual ~light();

			inline GLuint get_shadow_map() { return shadow_map; }

			// renders shadow map.
			virtual void render();

		private:
			GLuint shadow_map;
		};
	}
}
