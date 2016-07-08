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
            glm::scale(
                glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                glm::vec3(0.2f))
        ) {}

		int id;
    };
}
}
}
