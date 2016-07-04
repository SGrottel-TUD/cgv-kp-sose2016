#pragma once
#include "model_base.hpp"
#include <glm/glm.hpp>

namespace cgvkp {
    namespace rendering {
        namespace model {
            class graphic_model_base : public model_base {
            public:
				graphic_model_base() : model_matrix(1.0f), may_rotate(true) {}
				graphic_model_base(glm::mat4 model_matrix) : model_matrix(model_matrix) {}
                glm::mat4 model_matrix;
                bool may_rotate;
            };
        }
    }
}
