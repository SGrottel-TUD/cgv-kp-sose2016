#pragma once
#include "rendering/model/graphic_model_base.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
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

				virtual ~cloud_model() {}

				float speed;
				std::vector<float> speed_curve;
				int curve_iterator;
			};
		}
	}
}
