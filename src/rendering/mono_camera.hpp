#pragma once
#include "camera.hpp"

namespace cgvkp {
	namespace rendering {
		class mono_camera : public camera {
		public:
			explicit mono_camera(abstract_renderer* renderer);
			virtual ~mono_camera();

			virtual void set_position(glm::vec3 const& position);
			virtual void set_rotation(glm::quat const& rotation);
			virtual void set_scale(glm::vec3 const& scale);

			virtual void render();

		protected:
			glm::mat4x4 view_proj_matrix;
			glm::mat4x4 proj_matrix;
		};
	}
}
