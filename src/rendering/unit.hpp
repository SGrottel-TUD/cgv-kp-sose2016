#pragma once
#include "model.hpp"
#include "glm/gtc/quaternion.hpp"

namespace cgvkp {
	namespace rendering {
		// individual stars, hands and clouds.
		class unit {
		public:
			unit(model const& m) : mod(m), position(0, 0, 0), rotation(0, 0, 0, 1), scale(1, 1, 1) {}

			inline model const& get_model() const { return mod; }

			inline void set_position(glm::vec3 const& position) { this->position = position; }
			inline void set_rotation(glm::quat const& rotation) { this->rotation = rotation; }
			inline void set_scale(glm::vec3 const& scale) { this->scale = scale; }
			inline glm::vec3 const& get_position() const { return position; }
			inline glm::quat const& get_rotation() const { return rotation; }
			inline glm::vec3 const& get_scale() const { return scale; }

		protected:
			model const& mod;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};
	}
}
