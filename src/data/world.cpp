#include "data/world.hpp"
#include <cassert>
#include <iostream>

using namespace cgvkp;

data::world::world()
    : cfg(), hands_input(),
        in_hands(hands_input),
        input(cfg, tripple_buffer_facade<input_layer::hand_collection, 0>(hands_input)),
        hands(), hands_states(), stars(), score(0), next_hand_id(1), next_star_id(1) {
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

    std::vector<input_layer::hand>& in_buf = in_hands.buffer();

    assert(hands_states.size() == hands.size());
    assert(stars_states.size() == stars.size());

    for (hand_state_ptr hsp : hands_states) hsp->valid = false;

    const double pos_eps_sq(cfg.positional_epsilon() * cfg.positional_epsilon());
    for (input_layer::hand& in_hand : in_buf) {

        // search for the closest matching hand (greedy)
        std::shared_ptr<hand> h;
        double hd = 10000.0; // a suffiently large distance (100m) squared
        for (std::shared_ptr<hand> hi : hands) {
            double dx(hi->x - in_hand.x);
            double dy(hi->y - in_hand.y);
            double d(dx * dx + dy * dy);
            if (d > pos_eps_sq) continue; // too far apart
            if (d < hd) {
                hd = d;
                h = hi;
            }
        }

        if (h) {
            // update hand
            h->x = in_hand.x;
            h->y = in_hand.y;
            h->height = in_hand.h;
            hands_states[std::distance(hands.begin(), std::find(hands.begin(), hands.end(), h))]->valid = true;

        } else {
            // new hand
            hands.push_back(std::make_shared<hand>());
            h = hands.back();
            h->id = next_hand_id++;
            h->x = in_hand.x;
            h->y = in_hand.y;
            h->height = in_hand.h;
            h->star.reset();

            hands_states.push_back(std::make_shared<hand_state>());
            std::shared_ptr<hand_state> hs = hands_states.back();
            hs->valid = true;
        }

    }

    in_hands.valid() = false; // mark data consumed
}

void data::world::update_step(void) {

    // TODO: update data of hands and stars

}
