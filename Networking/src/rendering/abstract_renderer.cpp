#include "abstract_renderer.hpp"
#include "data/world.hpp"
#include "GL/glew.h"
#include <iostream>
#include <cassert>

using namespace cgvkp;

rendering::abstract_renderer::abstract_renderer(const ::cgvkp::data::world& data)
    : data(data), initialized(false) {
    // intentionally empty
}
rendering::abstract_renderer::~abstract_renderer() {
    assert(!initialized);
}
bool rendering::abstract_renderer::init(const window& wnd) {
    if (!initialized) {
        initialized = init_impl(wnd);
    }
    return initialized;
}
void rendering::abstract_renderer::deinit() {
    if (initialized) {
        deinit_impl();
        initialized = false;
    }
}