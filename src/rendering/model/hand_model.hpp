#pragma once
#include "rendering/model/graphic_model_base.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace cgvkp {
namespace rendering {
namespace model {
    class hand_model : public graphic_model_base {
    public:
        hand_model() : graphic_model_base(
            glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::vec3(0.50f))
        ) {}
        
        virtual ~hand_model() {}
    };
}
}
}
