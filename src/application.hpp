#pragma once
#include <memory>

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
        std::shared_ptr<rendering::window> debug_window;

    };

}
