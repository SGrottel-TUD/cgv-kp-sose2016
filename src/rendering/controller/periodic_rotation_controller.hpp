#pragma once
#include "rendering/model/graphic_model_base.hpp"
#include "rendering/controller/controller_base.hpp"

namespace cgvkp {
    namespace rendering {
        namespace controller {

            class periodic_rotation_controller : public controller_base {
            public:
                periodic_rotation_controller();
                virtual ~periodic_rotation_controller();
                virtual void update(double seconds, std::shared_ptr<abstract_user_input> input);
            private:
                glm::vec3 axis;
                double elapsed;
                float previous, period, amplitude;
            };
        }
    }
}
