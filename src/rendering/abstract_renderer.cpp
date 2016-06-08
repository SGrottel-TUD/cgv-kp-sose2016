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
bool rendering::abstract_renderer::init(const window& wnd, std::string const& resourcesBasePath) {
    if (!initialized) {
        initialized = init_impl(wnd, resourcesBasePath);
    }
    return initialized;
}
void rendering::abstract_renderer::deinit() {
    if (initialized) {
        deinit_impl();
        initialized = false;
    }
}