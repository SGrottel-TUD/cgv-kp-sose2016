#include "application.hpp"
#include "rendering/window.hpp"
#include "rendering/debug_renderer.hpp"
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
    std::shared_ptr<rendering::debug_renderer> debug_renderer;

    // create debug window
    debug_window = std::make_shared<rendering::window>(1280, 720);
    if (!debug_window || !debug_window->is_alive()) {
        debug_window.reset();
    } else {
        // init debug window
        debug_renderer = std::make_shared<rendering::debug_renderer>();
        if (!debug_renderer || !debug_renderer->init(*debug_window)) {
            std::cout << "Failed to create Debug renderer" << std::endl;
            debug_renderer.reset();
            debug_window.reset();
        }
    }

    // main loop
    while (debug_window) {

        //if (debug_window) { // this if is preparation for multiple windows
            // debug window is valid.
        if (debug_window->is_alive()) {
            debug_window->do_events();
            debug_renderer->render(*debug_window);
            debug_window->swap_buffers();

        } else { // window close is requested
            if (debug_renderer) {
                debug_renderer->deinit();
                debug_renderer.reset();
            }
            debug_window.reset();
        }
        //}

    }

    assert(!debug_renderer);
    assert(!debug_window);

}

void application::deinit() {
    // empty atm
}
