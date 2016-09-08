#pragma once
#include "vision/abstract_vision.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <random>

namespace cgvkp {
namespace vision {

    class dummy_vision : public abstract_vision {
    public:
        dummy_vision(data::input_layer& input_layer);
        virtual ~dummy_vision();

        virtual void update();
    protected:
        bool init_impl();
        void deinit_impl();
    private:
        typedef std::chrono::time_point<std::chrono::system_clock> time_point;
        struct hand_properties {
            float velocity;
            float goal_height;
            time_point start_time;
            time_point waiting_time;
            time_point recall_time;
        };
        std::vector<data::input_layer::hand> hands;
        std::vector<hand_properties> properties;
        float spawn_probability;
        std::default_random_engine rnd_engine;
        std::uniform_real_distribution<float> uniform_dist;
        std::normal_distribution<float> normal_dist;
        inline bool should_spawn_hand()
        {
            return spawn_probability >= uniform_dist(rnd_engine);
        }
        void spawn_hand(time_point start_time);
        void cleanup_data();
    };

}
}
