#include "release_renderer.hpp"
#include <iostream>

cgvkp::rendering::release_renderer::release_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
    models(), views(), controllers() {
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
    std::chrono::high_resolution_clock::time_point now_time = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(now_time - last_time).count();
    last_time = now_time;

    for (controller::controller_base::ptr controller : controllers) {
        if (!controller->has_model()) continue;
        controller->update(elapsed, wnd.get_user_input_object());
    }
    // TODO: Set up windows / viewports / etc. for stereo effect, execute multiple times if needed
    for (view::view_base::ptr view : views) {
        if (!view->is_valid()) continue;
        view->render();
    }
}