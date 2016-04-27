#pragma once
#include "glm/gtc/quaternion.hpp"

namespace cgvkp {
	namespace rendering {
		// forward declaration
		class abstract_renderer;

		class camera {
		public:
			virtual ~camera() {}

			virtual void set_position(glm::vec3 const& position) = 0;
			virtual void set_rotation(glm::quat const& rotation) = 0;
			virtual void set_scale(glm::vec3 const& scale) = 0;
			inline glm::vec3 const& get_position() const { return position; }
			inline glm::quat const& get_rotation() const { return rotation; }
			inline glm::vec3 const& get_scale() const { return scale; }

			virtual void render() = 0;

		protected:
			explicit camera(abstract_renderer* renderer);

			abstract_renderer* renderer;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};
	}
}
