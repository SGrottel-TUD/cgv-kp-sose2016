#pragma once
#include "camera.hpp"

namespace cgvkp {
	namespace rendering {
		class stereo_camera : public camera {
		public:
			explicit stereo_camera(abstract_renderer* renderer);
			virtual ~stereo_camera();

			virtual void set_position(glm::vec3 const& position);
			virtual void set_rotation(glm::quat const& rotation);
			virtual void set_scale(glm::vec3 const& scale);

			virtual void render();
		};
	}
}
