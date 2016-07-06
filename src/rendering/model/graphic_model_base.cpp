#include "rendering/model/graphic_model_base.hpp"

namespace cgvkp {
    namespace rendering {
        namespace model {
            graphic_model_base::graphic_model_base() :
                model_matrix(glm::mat4(1.0f)),
                may_rotate(true)
            {
                // intentionally empty
            }
            graphic_model_base::graphic_model_base(glm::mat4 model_matrix) :
                model_matrix(model_matrix)
            {
                // intentionally empty
            }

            graphic_model_base::~graphic_model_base() {
                // intentionally empty
            }
        }
    }
}
