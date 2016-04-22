#include "application.hpp"
#include "rendering/window.hpp"
#include "rendering/debug_renderer.hpp"
#include "rendering/debug_user_input.hpp"
#include "vision/dummy_vision.hpp"
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

    data.init();
    data.get_config().set_size(4.0f, 3.0f); // 4x3 meter game area
    data.get_config().set_positional_epsilon(0.1f); // 10cm positional precision (used for hand matching)

    start_time = std::chrono::high_resolution_clock::now();

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
        debug_renderer = std::make_shared<rendering::debug_renderer>(data);
        if (!debug_renderer || !debug_renderer->init(*debug_window)) {
            std::cout << "Failed to create Debug renderer" << std::endl;
            debug_renderer.reset();
            debug_window.reset();
        }
    }

    // create a vision component to be run in the main thread alongside the rendering
    //std::shared_ptr<vision::dummy_vision> vision = std::make_shared<vision::dummy_vision>(data.get_input_layer());

    std::shared_ptr<rendering::debug_user_input> vision = std::make_shared<rendering::debug_user_input>(data.get_input_layer());
    debug_window->set_user_input_object(vision);

    if (!vision->init()) {
        std::cout << "Failed to create Vision component" << std::endl;
        vision.reset();
    }

    // main loop
    uint64_t last_sim_frame = 0;
    while (debug_window) {

        std::chrono::duration<double> game_time_seconds(std::chrono::high_resolution_clock::now() - start_time);
        uint64_t game_time_sim_frames = static_cast<uint64_t>(game_time_seconds.count() * data.get_config().simulation_fps());

        // 1) vision in the main thread
        if (vision) {
            vision->update();
        }

        // 2) updating data model
        data.merge_input(); // merge input from the input_layer (vision) into the data model
        if ((last_sim_frame == 0) && (game_time_sim_frames > 0)) last_sim_frame = game_time_sim_frames - 1;
        while (last_sim_frame < game_time_sim_frames) {
            data.update_step(); // update the data (moving the star creatures)
            last_sim_frame++;
        }

        // 3) and now for the rendering

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

    // shutdown vision
    if (vision) {
        vision->deinit();
        vision.reset();
    }

    assert(!debug_renderer);
    assert(!debug_window);

}

void application::deinit() {
    // empty atm
}
