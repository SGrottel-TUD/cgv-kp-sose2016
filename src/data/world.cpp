#include "data/world.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

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
        if (in_hand.h < 0.0f) continue; // skip dead hands

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

    std::vector<hand_ptr>::iterator hi = hands.begin(), hands_end = hands.end();
    std::vector<hand_state_ptr>::iterator hsi = hands_states.begin();
    int idx = 0;
    while (hi != hands_end) {

        if (!(*hsi)->valid) {
            // TODO: retract hand quickly
            (*hi)->height = 0.0f;
        }

        if ((*hi)->height <= 0.0f) { // remove hand!

            // TODO: check for captured star creature

            hands.erase(hi);
            hands_states.erase(hsi);

            hands_end = hands.end();
            hi = hands.begin() + idx;
            hsi = hands_states.begin() + idx;
            continue;
        }

        // TODO: further update hand

        ++idx;
        ++hi;
        ++hsi;
    }



    // TODO: update data of hands and stars

    // A dummy star for debugging:
    if (stars.size() != 1) {
        stars.resize(1);
        stars_states.resize(1);
        stars[0] = std::make_shared<star>();
        stars_states[0] = std::make_shared<star_state>();
        stars[0]->id = next_star_id++;
    }
    double seconds_timer
        = static_cast<double>(std::chrono::system_clock::now().time_since_epoch().count())
        * static_cast<double>(std::chrono::system_clock::period::num)
        / static_cast<double>(std::chrono::system_clock::period::den);

    float cx = get_config().width() / 2.0f;
    float cy = get_config().height() / 2.0f;
    float rad = std::min<float>(cx / 2.0f, cy / 2.0f);

    stars[0]->height = 0.1f;
    stars[0]->in_hand = false;
    stars[0]->x = cx + static_cast<float>(std::cos(seconds_timer)) * rad;
    stars[0]->y = cy + static_cast<float>(std::sin(seconds_timer)) * rad;
    stars[0]->dx = -static_cast<float>(std::sin(seconds_timer));
    stars[0]->dy = static_cast<float>(std::cos(seconds_timer));

}
