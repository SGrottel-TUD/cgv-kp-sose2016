#pragma once
#include "data/world.hpp"
#include "application_config.hpp"
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
        data::world data;
        std::shared_ptr<rendering::window> debug_window;
        std::chrono::high_resolution_clock::time_point start_time;

    };

}
