#include "application.hpp"
#include "rendering/window.hpp"
#include "rendering/debug_renderer.hpp"
#include "rendering/release_renderer.hpp"
#include "rendering/model_listing_renderer.hpp"
#include "rendering/debug_user_input.hpp"
#include "vision/dummy_vision.hpp"
#include "util/resource_file.hpp"
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

    // Set the path for the resource_file class
    util::resource_file::resources_path = config.resourcesBasePath;

    return true;
}

void application::run()
{
	// create release window
	release_window = std::make_shared<rendering::window>(config.windowWidth, config.windowHeight);
	if (!release_window || !release_window->is_alive())
	{
		release_window.reset();
	}
	else
	{
		release_renderer = std::make_shared<rendering::release_renderer>(data);
		if (!release_renderer || !release_renderer->init(*release_window))
		{
			std::cout << "Failed to create Release renderer" << std::endl;
			release_renderer.reset();
			release_window.reset();
		}
		else
		{
			release_renderer->set_camera_mode(config.cameraMode);
			release_renderer->set_stereo_parameters(config.eyeSeparation, config.zZeroParallax);

			release_window->register_key_callback(GLFW_KEY_F, std::bind(&application::toggle_fullscreen, this), rendering::window::OnRelease);
			release_window->register_key_callback(GLFW_KEY_DOWN, std::bind(&application::increase_zzero_parallax, this, -0.1f), rendering::window::OnPress | rendering::window::OnRepeat);
			release_window->register_key_callback(GLFW_KEY_UP, std::bind(&application::increase_zzero_parallax, this, 0.1f), rendering::window::OnPress | rendering::window::OnRepeat);
			release_window->register_key_callback(GLFW_KEY_LEFT, std::bind(&application::increase_eye_separation, this, -0.001f), rendering::window::OnPress | rendering::window::OnRepeat);
			release_window->register_key_callback(GLFW_KEY_RIGHT, std::bind(&application::increase_eye_separation, this, 0.001f), rendering::window::OnPress | rendering::window::OnRepeat);
			release_window->register_key_callback(GLFW_KEY_M, std::bind(&application::set_camera_mode, this, rendering::mono));
			release_window->register_key_callback(GLFW_KEY_S, std::bind(&application::set_camera_mode, this, rendering::stereo));
		}
	}


    // create debug window
	if (config.debug)
	{
		debug_window = std::make_shared<rendering::window>(1280, 720, "CGV KP SoSe2016 - Debug");
		if (!debug_window || !debug_window->is_alive()) {
			debug_window.reset();
		}
		else {
			// init renderer selected in Project Property Sheet
			switch (config.active_renderer) {
			case application_config::renderers::models:
				debug_renderer = std::make_shared<rendering::model_listing_renderer>(data);
				break;
			default:
				debug_renderer = std::make_shared<rendering::debug_renderer>(data);
				break;
			}

			if (!debug_renderer || !debug_renderer->init(*debug_window)) {
				std::cout << "Failed to create Debug renderer" << std::endl;
				debug_renderer.reset();
				debug_window.reset();
			}
		}
	}

	std::shared_ptr<vision::abstract_vision> vision;
	if (debug_window)
	{
		auto debug_vision = std::make_shared<rendering::debug_user_input>(data.get_input_layer());
		vision = debug_vision;
		debug_window->set_user_input_object(debug_vision);
	}

    switch (config.active_vision) {
    case application_config::vision_inputs::dummy:
        // create a vision component to be run in the main thread alongside the rendering
        vision = std::make_shared<vision::dummy_vision>(data.get_input_layer());
        break;
    default:
        auto debug_vision = std::make_shared<rendering::debug_user_input>(data.get_input_layer());
        vision = debug_vision;
		if (debug_window)
		{
			debug_window->set_user_input_object(debug_vision);
		}
        break;
    }

	if (vision)
	{
		if (!vision->init()) {
			std::cout << "Failed to create Vision component" << std::endl;
			vision.reset();
		}
	}

    remote_renderer_server = std::make_shared<rendering::remote_renderer_server>(data);
    remote_renderer_server->init(*((release_window) ? release_window : debug_window));
    // main loop
    uint64_t last_sim_frame = 0;
    while (debug_window || release_window) {

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
        remote_renderer_server->render();
		if (debug_window)
		{
			if (debug_window->is_alive())
			{
				debug_window->do_events();
				debug_renderer->render(*debug_window);
				debug_window->swap_buffers();
			}
			else
			{
				if (debug_renderer)
				{
					debug_window->make_current();
					debug_renderer->deinit();
					debug_renderer.reset();
				}
				debug_window.reset();
			}
		}

		if (release_window)
		{
			if (release_window->is_alive())
			{
				release_window->do_events();
				release_renderer->render(*release_window);
				release_window->swap_buffers();
			}
			else
			{
				if (release_renderer)
				{
					release_window->make_current();
					release_renderer->deinit();
					release_renderer.reset();
				}
				release_window.reset();
			}
		}

    }
    remote_renderer_server->deinit();
    remote_renderer_server.reset();

    // shutdown vision
    if (vision) {
        vision->deinit();
        vision.reset();
    }

    assert(!remote_renderer_server);
    assert(!debug_renderer);
    assert(!debug_window);
	assert(!release_renderer);
	assert(!release_window);
}

void application::deinit() {
    // empty atm
}

void application::toggle_fullscreen()
{
	if (release_renderer)
	{
		release_window->make_current();
		release_renderer->lost_context();
		release_window->toggle_fullscreen();
		config.fullscreen = !config.fullscreen;
		release_renderer->restore_context(*release_window);
	}
}

void application::increase_eye_separation(float val)
{
	config.eyeSeparation += val;
	if (config.eyeSeparation < 0)
	{
		config.eyeSeparation = 0;
	}
	
	if(release_renderer)
	{
		release_renderer->set_stereo_parameters(config.eyeSeparation, config.zZeroParallax);
	}
}

void application::increase_zzero_parallax(float val)
{
	config.zZeroParallax += val;
	if (config.zZeroParallax < 0)
	{
		config.zZeroParallax = 0;
	}

	if (release_renderer)
	{
		release_renderer->set_stereo_parameters(config.eyeSeparation, config.zZeroParallax);
	}
}

void application::set_camera_mode(rendering::camera_mode mode)
{
	config.cameraMode = mode;

	if (release_renderer)
	{
		release_renderer->set_camera_mode(config.cameraMode);
	}
}
