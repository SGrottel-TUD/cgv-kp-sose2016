#include "data/input_layer.hpp"

using namespace cgvkp;

data::input_layer::input_layer(const world_config& config, input_hand_buffer& buf)
    : config(config), buf(std::move(buf)) {
    // intentionally empty
}
