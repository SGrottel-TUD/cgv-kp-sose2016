#pragma once
#include "rendering/model/graphic_model_base.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace cgvkp {
	namespace rendering {
		namespace model {
			class moon_model : public graphic_model_base {
			public:
				moon_model() : graphic_model_base(
					glm::translate(glm::vec3(2, 1, -5)) *
					glm::rotate(glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
					glm::rotate(glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
					glm::scale(glm::vec3(0.5f))
				) {}
			};
		}
	}
}
