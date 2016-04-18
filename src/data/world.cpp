#include "data/world.hpp"

using namespace cgvkp;

data::world::world()
    : cfg(), hands_input(),
        in_hands(hands_input),
        input(cfg, tripple_buffer_facade<input_layer::hand_collection, 0>(hands_input)),
        hands(), stars(), score(0), next_hand_id(1), next_star_id(1) {
}

data::world::~world() {
}

void data::world::init() {
    hands.clear();
    stars.clear();
    score = 0;
}

void data::world::merge_input(void) {
    in_hands.sync();
    if (!in_hands.valid()) return;

    // TODO: merge hands data

    in_hands.valid() = false; // mark data consumed
}

void data::world::update_step(void) {

    // TODO: update data of hands and stars

}
