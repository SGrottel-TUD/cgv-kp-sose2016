#pragma once
#include <chrono>
#include <memory>
#include "data/world.hpp"
#include "application_config.hpp"
#include "rendering/abstract_renderer.hpp"

namespace cgvkp {
	namespace rendering {
		class window;
	}

	class application {
	public:
		application();
		~application();

		bool init();
		void run();
		void deinit();

	private:
		void toggle_fullscreen();
		void increase_eye_separation(float val);
		void increase_zzero_parallax(float val);
		void set_camera_mode(rendering::release_renderer::CameraMode mode);

		application_config config;
		data::world data;
		std::shared_ptr<rendering::window> debug_window;
		std::shared_ptr<rendering::abstract_renderer> debug_renderer;
		std::shared_ptr<rendering::window> release_window;
		std::shared_ptr<rendering::release_renderer> release_renderer;
		std::chrono::high_resolution_clock::time_point start_time;
	};

}
