#include "data/world_config.hpp"

using namespace cgvkp;

data::world_config::world_config()
    : w(1.0f), h(1.0f),
    pos_eps(0.1f /* within 10cm considered the same location*/),
    sim_fps(60.0) {
}

data::world_config::~world_config() {
}
