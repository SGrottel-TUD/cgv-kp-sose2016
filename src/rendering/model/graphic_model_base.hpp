#pragma once
#include "model_base.hpp"
#include <glm/glm.hpp>

namespace cgvkp {
    namespace rendering {
        namespace model {

            class graphic_model_base : public model_base {
            public:
                graphic_model_base();
                graphic_model_base(glm::mat4 model_matrix);
                virtual ~graphic_model_base();
                glm::mat4 model_matrix;
            };
        }
    }
}
