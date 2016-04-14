#pragma once
#include "rendering/window.hpp"

namespace cgvkp {
namespace rendering {

    class debug_renderer {
    public:
        debug_renderer();
        ~debug_renderer();

        bool init(const window& wnd);
        void render(const window& wnd);
        void deinit();
    };

}
}
