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
                std::cout << "Removed some star" << std::endl;
                continue;
            }
            if (data_pair->elapsed <= data_pair->anim_duration)
            {
                // Linear interpolation from one matrix to another.
                float t = (float)(std::fmin(data_pair->anim_duration, data_pair->elapsed) / data_pair->anim_duration);
                std::cout << t << "\t";
                hand->model_matrix = data_pair->hand_start +
                    (data_pair->hand_target - data_pair->hand_start) * t;
                star->model_matrix = data_pair->star_start +
                    (data_pair->star_target - data_pair->star_start) * t;
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
        // TODO set target matrixes
    }
}
}
}
