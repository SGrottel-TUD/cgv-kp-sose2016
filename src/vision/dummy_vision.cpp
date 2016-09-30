#include "dummy_vision.hpp"
#include <cassert>
#include <iostream>

using namespace cgvkp;

vision::dummy_vision::dummy_vision(data::input_layer& input_layer)
    : abstract_vision(input_layer),
    hands(),
    properties(),
    spawn_probability(3.0f / (30.0f * 10.0f)), // Aprox.: 3 every 10 seconds
    uniform_dist(),
    normal_dist(0.5f, 1.0f/6.0f),
    rnd_engine()
{
    // nothing to do here
}

vision::dummy_vision::~dummy_vision() {
    // nothing to do here
}

bool vision::dummy_vision::init_impl() {

    // nothing to do here

    return true;
}

void vision::dummy_vision::update() {
    auto seconds_timer = std::chrono::system_clock::now();

    std::vector<data::input_layer::hand>& buf = input_layer.buffer();

    // Cleanup data
    cleanup_data();
    // Generate new hand if needed
    if (should_spawn_hand())
        spawn_hand(seconds_timer);
    // Ensure arrays are sane
    assert(hands.size() == properties.size());

    // Update hands / animate
    {
        for (size_t i = 0u; i < hands.size(); ++i)
        {
            auto prop = properties[i];
            if (seconds_timer < prop.waiting_time)
                hands[i].h = (seconds_timer - prop.start_time).count() * prop.velocity;
            else
            {
                if (seconds_timer > prop.recall_time)
                    hands[i].h = prop.goal_height - (seconds_timer - prop.recall_time).count() * prop.velocity;
                else
                    hands[i].h = prop.goal_height;
            }
        }
    }

    // Write to input_layer
    buf.resize(hands.size());
    std::memcpy(buf.data(), hands.data(), sizeof(data::input_layer::hand) * hands.size());

    input_layer.sync_buffer();
}

void vision::dummy_vision::deinit_impl() {
    // nothing to do here
}

void vision::dummy_vision::cleanup_data() {
    for (int i = (int)hands.size() - 1; i >= 0; --i)
    {
        if (hands[i].h < 0.0f)
        {
            hands.erase(hands.begin() + i);
            properties.erase(properties.begin() + i);
        }
    }
}
void vision::dummy_vision::spawn_hand(time_point start_time) {
    data::input_layer::hand hand;
    hand_properties prop;
    hand.h = 0.0f;
    hand.x = normal_dist(rnd_engine) * input_layer.get_config().width();
    hand.y = normal_dist(rnd_engine) * input_layer.get_config().height();

    prop.goal_height = 0.2f + normal_dist(rnd_engine) * (0.4f);
    float waiting_time = 0.0f + normal_dist(rnd_engine) * 2.5f; // [0, 5] seconds
    float up_time = 0.3f + normal_dist(rnd_engine) * 0.5f; // [0.3, 1.3] seconds

    prop.start_time = start_time;
    prop.waiting_time = prop.start_time;
    prop.waiting_time += std::chrono::milliseconds((int)(up_time * 1000));
    prop.recall_time = prop.waiting_time;
    prop.recall_time += std::chrono::milliseconds((int)(waiting_time * 1000));
    prop.velocity = prop.goal_height / (prop.waiting_time - prop.start_time).count();

    hands.push_back(hand);
    properties.push_back(prop);
}