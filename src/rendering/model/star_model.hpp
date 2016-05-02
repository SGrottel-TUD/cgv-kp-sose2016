#pragma once
#include "rendering/model/model_base.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace cgvkp {
namespace rendering {
namespace model {
    class star_model : public model_base {
    public:
        star_model() : model_matrix(glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))) {}
        
        virtual ~star_model() {}
        glm::mat4 model_matrix;
    };
}
}
}
