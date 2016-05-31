#include "rendering/controller/data_controller.hpp"

#include "rendering/view/star_view.hpp"
#include "rendering/view/hand_view.hpp"

#include <glm/gtx/transform.hpp>

#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif

namespace cgvkp {
namespace rendering {
namespace controller {
    data_controller::data_controller(release_renderer* renderer, const data::world &data) :
        controller_base(), renderer(renderer), data(data), stars(), hands()
    {
        // intentionally empty
    }

    data_controller::~data_controller() {
        // intentionally empty
    }

    bool data_controller::has_model() const {
        return true;
    }
    void data_controller::update(double seconds, std::shared_ptr<abstract_user_input> input)
    {
        // Iterate through stars
        for (auto data_star : data.get_stars())
        {
            if (stars.count(data_star->id) == 0) {
#if defined(_DEBUG) || defined(DEBUG)
                std::cout << "Creating star " << data_star->id << std::endl;
#endif
                // We need to create this star
                auto star = std::make_shared<model::star_model>();
                renderer->add_model(star);

                // And its view
                auto starView = std::make_shared<view::star_view>();
                starView->set_model(star);
                starView->init();
                renderer->add_view(starView);

                // And add it to the map
                stars.insert(std::make_pair(data_star->id, star));
#if defined(_DEBUG) || defined(DEBUG)
                std::cout << stars.size() << " stars." << std::endl;
#endif
            }
            // Now we are sure we know about the star
            // Update its data
            std::shared_ptr<model::star_model> star = stars[data_star->id].lock();
            star->model_matrix[3][0] = data_star->x;
            star->model_matrix[3][1] = data_star->y;
            star->model_matrix[3][2] = data_star->height;
        }
        // Iterate through hands
        for (auto data_hand : data.get_hands())
        {
            if (hands.count(data_hand->id) == 0) {
#if defined(_DEBUG) || defined(DEBUG)
                std::cout << "Creating hand " << data_hand->id << std::endl;
#endif
                // We need to create this hand 
                auto hand = std::make_shared<model::hand_model>();
                renderer->add_model(hand);

                // And its view
                auto handView = std::make_shared<view::hand_view>();
                handView->set_model(hand);
                handView->init();
                renderer->add_view(handView);

                // And add it to the map
                hands.insert(std::make_pair(data_hand->id, hand));
#if defined(_DEBUG) || defined(DEBUG)
                std::cout << hands.size() << " hands." << std::endl;
#endif
            }
            // Now we are sure we know about the hand
            // Update its data
            std::shared_ptr<model::hand_model> hand = hands[data_hand->id].lock();
            hand->model_matrix[3][0] = data_hand->x;
            hand->model_matrix[3][1] = data_hand->y;
            hand->model_matrix[3][2] = data_hand->height;
        }
    }
}
}
}
