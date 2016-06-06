#pragma once

#include "controller_base.hpp"
#include "rendering/model/star_model.hpp"
#include "rendering/model/hand_model.hpp"
#include <memory>
#include <vector>

namespace cgvkp {
    namespace rendering {
        namespace controller {

            class caught_star_controller : public controller_base {
            public:
                caught_star_controller();
                virtual ~caught_star_controller();

                // Answer if the object has a model
                virtual bool has_model() const;

                /**
                * @param seconds Time elapsed since last call, in seconds
                */
                virtual void update(double seconds, std::shared_ptr<abstract_user_input> input);

                // Add caught pair
                void caught_pair(std::shared_ptr<model::hand_model> hand, std::shared_ptr<model::star_model> star);
            private:
                struct caught_pair_data {
                    caught_pair_data(std::shared_ptr<model::hand_model> hand, std::shared_ptr<model::star_model> star);
                    double elapsed = 0.0f;
                    std::weak_ptr<model::hand_model> hand;
                    std::weak_ptr<model::star_model> star;
                    double anim_duration = 1.0;
                    glm::mat4 hand_start, star_start;
                    glm::mat4 hand_target = glm::mat4(1.0f);
                    glm::mat4 star_target = glm::mat4(1.0f);
                };
                std::vector<caught_pair_data> data;
            };

        }
    }
}
