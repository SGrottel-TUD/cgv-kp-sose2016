#pragma once
#include "data/world.hpp"
#include "application_config.hpp"
#include "rendering/abstract_renderer.hpp"
#include <memory>
#include <chrono>

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

        application_config config;

    private:
		void toggle_fullscreen();

        data::world data;
        std::shared_ptr<rendering::window> debug_window;
		std::shared_ptr<rendering::abstract_renderer> renderer;
        std::chrono::high_resolution_clock::time_point start_time;
    };

}
