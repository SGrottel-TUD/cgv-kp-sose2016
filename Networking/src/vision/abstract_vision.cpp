#include "abstract_vision.hpp"
#include <cassert>

using namespace cgvkp;

vision::abstract_vision::abstract_vision(data::input_layer& input_layer)
    : input_layer(input_layer), initialized(false) {
    // intentionally empty
}
vision::abstract_vision::~abstract_vision() {
    assert(!initialized);
}
bool vision::abstract_vision::init() {
    if (!initialized) {
        initialized = init_impl();
    }
    return initialized;
}
void vision::abstract_vision::deinit() {
    if (initialized) {
        deinit_impl();
        initialized = false;
    }
}