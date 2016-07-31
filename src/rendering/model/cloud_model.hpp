#pragma once
#include "rendering/model/graphic_model_base.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

namespace cgvkp {
	namespace rendering {
		namespace model {
			class cloud_model : public graphic_model_base {
			public:
				cloud_model() : graphic_model_base(glm::scale(glm::vec3(0.2f))){}
				cloud_model(float scale) : graphic_model_base(
					glm::scale(glm::vec3(scale))
				) {}

				float speed;
				std::vector<float> speed_curve;
				int curve_iterator;
				glm::vec3 position;
				glm::quat rotation;
				glm::vec3 scale;
				glm::vec3 velocity;
			};
		}
	}
}
