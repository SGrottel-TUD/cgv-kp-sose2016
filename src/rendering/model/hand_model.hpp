#pragma once
#include "rendering/model/model_base.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace cgvkp {
namespace rendering {
namespace model {
    class hand_model : public model_base {
    public:
        hand_model() : model_matrix(glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))) {}
        
        virtual ~hand_model() {}
        glm::mat4 model_matrix;
    };
}
}
}
