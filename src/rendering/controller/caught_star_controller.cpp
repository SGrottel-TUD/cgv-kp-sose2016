#include "caught_star_controller.hpp"
#include <iostream>
namespace cgvkp {
namespace rendering {
namespace controller {
    caught_star_controller::caught_star_controller() : data()
    { // Intentionally empty
    }
    caught_star_controller::~caught_star_controller() {}
    bool caught_star_controller::has_model() const { return true; }
    void caught_star_controller::caught_pair(std::shared_ptr<model::hand_model> hand, std::shared_ptr<model::star_model> star)
    {
        // Check if already added
        auto it = data.begin();
        while (it != data.end())
        {
            if (it->hand.lock() == hand)
                break;
            ++it;
        }
        if (it == data.end())
            data.push_back(caught_pair_data(hand, star));
    }
    void caught_star_controller::update(double seconds, std::shared_ptr<abstract_user_input> input)
    {
        for (auto data_pair = data.begin(); data_pair != data.end();)
        {
            auto hand = data_pair->hand.lock();
            auto star = data_pair->star.lock();
            if (!hand || !star)
            {
                data_pair = data.erase(data_pair);
                continue;
            }
            if (data_pair->elapsed <= data_pair->anim_duration)
            {
                // Linear interpolation from one matrix to another.
                float t = (float)(std::fmin(data_pair->anim_duration, data_pair->elapsed) / data_pair->anim_duration);
                glm::vec3 hand_pos = glm::vec3(hand->model_matrix[3][0], hand->model_matrix[3][1], hand->model_matrix[3][2]);
                hand->model_matrix = data_pair->hand_start +
                    (data_pair->hand_target - data_pair->hand_start) * t;
                hand->model_matrix[3] = glm::vec4(hand_pos, 1.0f);
                glm::vec3 star_pos = glm::vec3(star->model_matrix[3][0], star->model_matrix[3][1], star->model_matrix[3][2]);
                star->model_matrix = data_pair->star_start +
                    (data_pair->star_target - data_pair->star_start) * t;
                star->model_matrix[3] = glm::vec4(star_pos, 1.0f);
                data_pair->elapsed += seconds;
            }
            // Continue with next data pair
            ++data_pair;
        }
    }

    caught_star_controller::caught_pair_data::caught_pair_data(
        std::shared_ptr<model::hand_model> hand,
        std::shared_ptr<model::star_model> star) :
        hand(hand), star(star), hand_start(hand->model_matrix), star_start(star->model_matrix)
    {
        hand_target =
            glm::scale(glm::vec3(0.05f)) *
            glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::rotate(glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        star_target =
            glm::scale(
                glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                glm::vec3(0.18f)) *
            glm::rotate(glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
}
}
}
