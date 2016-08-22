#pragma once
#include "rendering/model/graphic_model_base.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace cgvkp {
namespace rendering {
namespace model {
    class star_model : public graphic_model_base {
    public:
        star_model() : graphic_model_base(
            glm::scale(glm::vec3(0.2f))
        ) {}

		int id;
    };
}
}
}
