#include "dummy_vision.hpp"
#include <chrono>

using namespace cgvkp;

vision::dummy_vision::dummy_vision(data::input_layer& input_layer)
    : abstract_vision(input_layer) {
    // nothing to do here
}

vision::dummy_vision::~dummy_vision() {
    // nothing to do here
}

bool vision::dummy_vision::init_impl() {

    // nothing to do here

    return true;
}

void vision::dummy_vision::update() {
    double seconds_timer
        = static_cast<double>(std::chrono::system_clock::now().time_since_epoch().count())
        * static_cast<double>(std::chrono::system_clock::period::num)
        / static_cast<double>(std::chrono::system_clock::period::den);

    std::vector<data::input_layer::hand>& buf = input_layer.buffer();

    buf.resize(1); // one demo hand ...

    // ... in the center of the game area ...
    buf[0].x = input_layer.get_config().width() / 2.0f;
    buf[0].y = input_layer.get_config().height() / 2.0f;

    // ... moving up and down.
    buf[0].h = 0.1f + 0.8f * static_cast<float>(0.5 + 0.5 * std::sin(seconds_timer));

    // and that is it.
    input_layer.sync_buffer();
}

void vision::dummy_vision::deinit_impl() {
    // nothing to do here
}
