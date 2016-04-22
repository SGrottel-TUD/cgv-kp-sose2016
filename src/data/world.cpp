#include "data/world.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace cgvkp;

const float data::world::hand_state::retraction_accel = 0.02f;

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
            hs->retract_speed = 0.0f;
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
            // retract hand quickly
            (*hsi)->retract_speed += hand_state::retraction_accel;
            (*hi)->height -= (*hsi)->retract_speed;
        } else {
            (*hsi)->retract_speed = 0.0f;
        }

        if ((*hi)->height <= 0.0f) { // remove hand!

            if ((*hi)->star) {
                // remove captured star and increase score

                ptrdiff_t si = std::distance(stars.begin(), std::find(stars.begin(), stars.end(), (*hi)->star));
                stars.erase(stars.begin() + si);
                stars_states.erase(stars_states.begin() + si);

                score++;
                std::cout << "Score++ = " << score << std::endl;

            }

            hands.erase(hi);
            hands_states.erase(hsi);

            hands_end = hands.end();
            hi = hands.begin() + idx;
            hsi = hands_states.begin() + idx;
            continue;
        }

        if (!(*hi)->star) {
            // Try capture star
            std::shared_ptr<star> cs;
            double csd = 10000.0;
            const double pos_eps_sq(cfg.positional_epsilon() * cfg.positional_epsilon());

            for (std::shared_ptr<star> si : stars) {
                double dx(si->x - (*hi)->x);
                double dy(si->y - (*hi)->y);
                double d(dx * dx + dy * dy);
                if (d > pos_eps_sq) continue; // too far apart
                if (si->height - cfg.positional_epsilon() > (*hi)->height) continue; // star too high
                if (csd > d) {
                    // star could be captured
                    csd = d;
                    cs = si;
                }
            }

            if (cs) {
                // capture star!
                (*hi)->star = cs;
                cs->in_hand = true;
            }
        }

        if ((*hi)->star) {
            (*hi)->star->x = (*hi)->x;
            (*hi)->star->y = (*hi)->y;
            (*hi)->star->height = (*hi)->height;
        }

        ++idx;
        ++hi;
        ++hsi;
    }



    // TODO: update data of hands and stars

    // A dummy stars for debugging:
    static int starmakecounter = 0;
    starmakecounter++;
    if (starmakecounter > 100) {
        starmakecounter = 0;

        if (stars.size() < 10) {
            stars.push_back(std::make_shared<star>());
            stars_states.push_back(std::make_shared<star_state>());

            std::shared_ptr<star> s = stars.back();
            s->id = next_star_id++;
            s->x = (static_cast<float>(::rand()) / static_cast<float>(RAND_MAX)) * get_config().width();
            s->y = (static_cast<float>(::rand()) / static_cast<float>(RAND_MAX)) * get_config().height();
            s->dx = 0.0f;
            s->dy = -1.0f;
            s->height = 0.7f;
            s->in_hand = false;

        }
    }

}
