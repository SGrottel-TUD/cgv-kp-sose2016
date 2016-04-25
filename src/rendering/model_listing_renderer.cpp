#include "model_listing_renderer.hpp"
#include <iostream>

cgvkp::rendering::model_listing_renderer::model_listing_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data) {
}
cgvkp::rendering::model_listing_renderer::~model_listing_renderer() {}

bool cgvkp::rendering::model_listing_renderer::init_impl(const window& wnd) {
    std::cout << "Model listing renderer initialized" << std::endl;
    return true;
}
void cgvkp::rendering::model_listing_renderer::deinit_impl() {
}

void cgvkp::rendering::model_listing_renderer::render(const window& wnd) {
    // TODO: Implement release_renderer::render
}