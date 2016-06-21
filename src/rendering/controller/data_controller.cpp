#include "rendering/controller/data_controller.hpp"

#include "rendering/view/star_view.hpp"
#include "rendering/view/hand_view.hpp"
#include "rendering/controller/periodic_rotation_controller.hpp"

#include <glm/gtx/transform.hpp>

#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif

namespace cgvkp {
namespace rendering {
namespace controller {
    data_controller::data_controller(release_renderer* renderer, const data::world &data) :
        controller_base(), renderer(renderer), data(data), stars(), hands(),
        caught_star_controller(std::make_shared<controller::caught_star_controller>())
    {
        renderer->add_controller(caught_star_controller);
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
        auto active_models = std::vector<int>();
        for (auto data_star : data.get_stars())
        {
            active_models.push_back(data_star->id);
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

                // And its controller
                auto starController = std::make_shared<controller::periodic_rotation_controller>();
                starController->set_model(star);
                renderer->add_controller(starController);

                // And add it to the map
                stars.insert(std::make_pair(data_star->id, star));
#if defined(_DEBUG) || defined(DEBUG)
                std::cout << stars.size() << " stars." << std::endl;
#endif
            }
            // Now we are sure we know about the star
            // Update its data
            std::shared_ptr<model::star_model> star = stars[data_star->id].lock();
            star->model_matrix[3].x = data_star->x;
            star->model_matrix[3].y = trans_height(data_star->height);
            star->model_matrix[3].z = -data_star->y;

            // Check if it was caught
            if (data_star->in_hand) {
                star->may_rotate = false;
            }
        }
        // Deactivate models of disappeared stars
        for (auto it = stars.begin(), ite = stars.end(); it != ite;)
        {
            if (std::find(active_models.begin(), active_models.end(), it->first) == active_models.end())
            {
                // Model is not active, deactivate it
                renderer->remove_model(it->second.lock());
                it = stars.erase(it);
            }
            else
            {
                ++it;
            }
        }
        active_models.clear();
        // Iterate through hands
        for (auto data_hand : data.get_hands())
        {
            active_models.push_back(data_hand->id);
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

                // And its controller
                auto handController = std::make_shared<controller::periodic_rotation_controller>();
                handController->set_model(hand);
                renderer->add_controller(handController);

                // And add it to the map
                hands.insert(std::make_pair(data_hand->id, hand));
#if defined(_DEBUG) || defined(DEBUG)
                std::cout << hands.size() << " hands." << std::endl;
#endif
            }
            // Now we are sure we know about the hand
            // Update its data
            std::shared_ptr<model::hand_model> hand = hands[data_hand->id].lock();
            hand->model_matrix[3].x = data_hand->x;
            hand->model_matrix[3].y = trans_height(data_hand->height);
            hand->model_matrix[3].z = -data_hand->y;

            // Check if it was caught
            if (data_hand->star != nullptr)
            {
                hand->may_rotate = false;
                auto star = stars[data_hand->star->id].lock();
                if (star)
                    caught_star_controller->caught_pair(hand, star);
            }
        }
        // Deactivate models of disappeared hands
        for (auto it = hands.begin(), ite = hands.end(); it != ite;)
        {
            if (std::find(active_models.begin(), active_models.end(), it->first) == active_models.end())
            {
                // Model is not active, deactivate it
                renderer->remove_model(it->second.lock());
                it = hands.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
}
}
}
