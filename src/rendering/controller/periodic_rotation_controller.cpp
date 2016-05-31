#include "periodic_rotation_controller.hpp"
#include <memory>
#include <random>
#include <glm/gtx/transform.hpp>

namespace cgvkp {
    namespace rendering {
        namespace controller {
            periodic_rotation_controller::periodic_rotation_controller() :
                axis(glm::vec3(0.0f, 1.0f, 0.0f)),
                elapsed(0.0), amplitude(0.75f * glm::quarter_pi<float>()), period(5.0f), previous(0.0f)
            {
                std::random_device r;
                std::default_random_engine random_engine(std::seed_seq{ r(), r(), r() });
                // Make axis random
                std::uniform_real_distribution<float> uniform(-1.0f, 1.0f);
                axis.x = uniform(random_engine);
                axis = glm::normalize(axis);
                // Also period
                uniform = std::uniform_real_distribution<float>(2.0f, 6.0f);
                period = uniform(random_engine);
                // And amplitude
                uniform = std::uniform_real_distribution<float>(0.5f, 1.0f);
                amplitude = uniform(random_engine) * glm::quarter_pi<float>();
            }
            periodic_rotation_controller::~periodic_rotation_controller()
            {
                // Intentionally empty
            }
            void periodic_rotation_controller::update(double seconds, std::shared_ptr<abstract_user_input> input)
            {
                auto m = std::dynamic_pointer_cast<model::graphic_model_base>(model.lock());
                if (!m) return;

                elapsed += seconds;
                float new_value = glm::sin(glm::two_pi<float>() * elapsed / period) * amplitude;
                float rotation_angle = new_value - previous;

                glm::vec3 position = glm::vec3(m->model_matrix[3][0], m->model_matrix[3][1], m->model_matrix[3][2]);

                m->model_matrix =
                    glm::translate(position) *
                    glm::rotate(rotation_angle, axis) *
                    glm::translate(-position) *
                    m->model_matrix;

                previous = new_value;
            }
        }
    }
}