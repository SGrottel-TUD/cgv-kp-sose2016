#include "application.hpp"
#include "rendering/window.hpp"
#include "GLFW/glfw3.h"
#include <cassert>
#include <iostream>

using namespace cgvkp;

application::application() : debug_window(nullptr) {
}

application::~application() {
    assert(!debug_window);
}

bool application::init() {
    if (::glfwInit() != GL_TRUE) {
        std::cerr << "glfwInit failed" << std::endl;
        return false;
    }
    return true;
}

void application::run() {

    debug_window = std::make_shared<rendering::window>(1280, 720);
    if (!debug_window || !debug_window->is_alive()) {
        debug_window.reset();
    }

    debug_window->make_current();
    ::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    ::glEnable(GL_DEPTH_TEST);
    ::glEnable(GL_CULL_FACE);

    // main loop
    while ((debug_window) && (debug_window->is_alive())) {
        debug_window->do_events();
        debug_window->make_current();
        //main_window->update_size(scene.camera());
        //main_window->update_cursor(scene.cursor());

        //manage_views_and_controllers();
        //update_models();

        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //render_views();

        debug_window->swap_buffers();
    }


    debug_window.reset(); // deletes the window object

}

void application::deinit() {
}
