#include "release_renderer.hpp"
#include <iostream>

cgvkp::rendering::release_renderer::release_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data) {
}
cgvkp::rendering::release_renderer::~release_renderer() {}

bool cgvkp::rendering::release_renderer::init_impl(const window& wnd) {
    // TODO: Implement release_renderer::init_impl
    std::cout << "Release renderer initialized" << std::endl;
    return true;
}
void cgvkp::rendering::release_renderer::deinit_impl() {
}

void cgvkp::rendering::release_renderer::render(const window& wnd) {
    // TODO: Implement release_renderer::render
}