#pragma once
#include "rendering/window.hpp"

namespace cgvkp {
namespace data {
    class world;
}
namespace rendering {

    class debug_renderer {
    public:
        debug_renderer(const data::world& data);
        ~debug_renderer();

        bool init(const window& wnd);
        void render(const window& wnd);
        void deinit();
    private:
        const data::world& data;
    };

}
}
