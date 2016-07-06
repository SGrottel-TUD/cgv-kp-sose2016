#include "data/world.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <chrono>

using namespace cgvkp;

const float data::world::hand_state::retraction_accel = 0.02f;

data::world::world()
    : cfg(), hands_input(),
        in_hands(hands_input),
        input(cfg, tripple_buffer_facade<input_layer::hand_collection, 0>(hands_input)),
        hands(), hands_states(), stars(), score(0), next_hand_id(1), next_star_id(1),
        mode(game_mode::stopped), star_spawn_timer(0),
        rnd_engine(), rnd_width(), rnd_height(), rnd_elevation() {
}

data::world::~world() {
}

void data::world::init() {
    hands.clear();
    stars.clear();
    rnd_engine.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    score = 0;
}

void data::world::set_game_mode(game_mode new_mode) {
    // for now:
    mode = new_mode;
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

    // update data of stars

    const size_t max_stars_count = 10;
    const size_t make_stars_frame_count = 60; // make a star every 60 sim. frames
    const double star_life_time = 30.0; // 30 seconds
    const double star_min_z = 0.1;
    const double star_max_z = 0.9;
    const double star_mean_z = 0.3;
    const double star_sigma_z = 0.1;
    const double star_off_z = 1.5;
    const unsigned int star_path_ctrl_pts = 15;

    star_spawn_timer++;
    if (star_spawn_timer > make_stars_frame_count) {
        star_spawn_timer = 0;

        if (stars.size() < max_stars_count) {
            // add a new star
            std::shared_ptr<star> new_star = std::make_shared<star>();
            stars.push_back(new_star);

            new_star->id = next_star_id++;

            std::shared_ptr<star_state> new_star_state = std::make_shared<star_state>();
            stars_states.push_back(new_star_state);

            rnd_width = std::uniform_real_distribution<double>(0.0, static_cast<double>(get_config().width()));
            rnd_height = std::uniform_real_distribution<double>(0.0, static_cast<double>(get_config().height()));
            rnd_elevation = std::normal_distribution<double>(star_mean_z, star_sigma_z);

            std::vector<glm::dvec3> spline;
            for (unsigned int j = 0; j < star_path_ctrl_pts; ++j) {
                double x = rnd_width(rnd_engine);
                double y = rnd_height(rnd_engine);
                double z = 0.0;
                while ((z < star_min_z) || (z > star_max_z)) z = rnd_elevation(rnd_engine);

                spline.push_back(glm::dvec3(x, y, z));
            }

            spline.front().z = star_off_z;
            spline.back().z = star_off_z;

            new_star_state->t = 0.0;
            new_star_state->speed = 1.0 / (get_config().simulation_fps() * star_life_time);
            new_star_state->path.set_control_points(spline);

        }

    }

    size_t star_cnt = stars.size();
    for (size_t star_i = 0; star_i < star_cnt; ) {
        std::shared_ptr<star> obj = stars[star_i];
        if (obj->in_hand) {
            ++star_i;
            continue; // captured stars do not move any more
        }
        std::shared_ptr<star_state> state = stars_states[star_i];

        if (state->t >= 1.0) {
            // remove star
            stars.erase(stars.begin() + star_i);
            stars_states.erase(stars_states.begin() + star_i);
            star_cnt--;
            continue;
        }

        glm::dvec2 dir(obj->x, obj->y);
        glm::dvec3 pos = state->path.evaluate(state->t);
        dir = glm::normalize(glm::dvec2(pos) - dir);

        state->t += state->speed;

        obj->x = static_cast<float>(pos.x);
        obj->y = static_cast<float>(pos.y);
        obj->height = static_cast<float>(pos.z);
        obj->dx = static_cast<float>(dir.x);
        obj->dy = static_cast<float>(dir.y);

        ++star_i;
    }

}
