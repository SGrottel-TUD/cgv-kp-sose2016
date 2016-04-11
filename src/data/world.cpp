#include "data/world.h"

using namespace cgvkp;

data::world::world() 
    : cfg(), hands_input(), 
    input(cfg, tripple_buffer_facade<input_layer::hand_collection,0>(hands_input)) {
}

data::world::~world() {
}

void data::world::init() {
}
