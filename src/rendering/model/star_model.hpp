#pragma once
#include "rendering/model/model_base.hpp"
#include <glm/glm.hpp>

namespace cgvkp {
namespace rendering {
namespace model {
    class star_model : public model_base {
    public:
        star_model() : model_matrix(glm::mat4(1.0f)) {}
        virtual ~star_model() {}
        glm::mat4 model_matrix;
    };
}
}
}
