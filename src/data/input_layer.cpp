#include "data/input_layer.h"
#include "glm/glm.hpp"
#include <cfloat>

using namespace cgvkp;

data::input_layer::hand::~hand() {
    // intentionally empty
}

void data::input_layer::hand::set_height(float h) {
    std::lock_guard<std::mutex> lock(sync);
    height = h;
}

data::input_layer::hand::hand(std::mutex& sync, unsigned int id, float x, float y)
    : sync(sync), id(id), x(x), y(y), height(0.0f) {
    // intentionally empty
}

data::input_layer::input_layer(const world_config& config)
    : config(config), sync(), next_hand_id(1), hands() {
    // intentionally empty
}

data::input_layer::~input_layer() {
    // intentionally empty
}

unsigned int data::input_layer::hand_count() {
    std::lock_guard<std::mutex> lock(sync);
    return hands.size();
}

data::input_layer::hand_ptr data::input_layer::get_hand_from_index(unsigned int idx) {
    std::lock_guard<std::mutex> lock(sync);
    std::list<hand_ptr>::iterator i = hands.begin();
    std::advance(i, idx);
    return *i;
}

data::input_layer::hand_ptr data::input_layer::get_hand_from_id(unsigned int id) {
    std::lock_guard<std::mutex> lock(sync);
    for (hand_ptr h : hands) {
        if (h->get_id() == id) return h;
    }
    return nullptr;
}

data::input_layer::hand_ptr data::input_layer::find_hand_at(float x, float y, float pos_eps) {
    std::lock_guard<std::mutex> lock(sync);
    glm::vec2 qp(x, y);
    hand_ptr min_h;
    float min_l = FLT_MAX;
    for (hand_ptr h : hands) {
        float l = glm::length(glm::vec2(h->get_x(), h->get_y()) - qp);
        if (l < pos_eps) {
            if (min_l > l) {
                min_l = l;
                min_h = h;
            }
        }
    }
    return min_h;

}

data::input_layer::hand_ptr data::input_layer::add_hand_at(float x, float y, float pos_eps) {
    std::lock_guard<std::mutex> lock(sync);
    hand_ptr h = find_hand_at(x, y, pos_eps);
    if (!h) h.reset(new hand(sync, next_hand_id++, x, y));

    return h;
}

void data::input_layer::begin_sync() {
    sync.lock();
    // start data sync
}

void data::input_layer::end_sync() {
    // end data sync
    sync.unlock();
}
