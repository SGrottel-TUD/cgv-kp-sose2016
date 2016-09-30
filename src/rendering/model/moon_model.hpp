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
					glm::translate(glm::vec3(4, 2, -10))
				) {}
			};
		}
	}
}
