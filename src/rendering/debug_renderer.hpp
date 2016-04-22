#pragma once
#include "rendering/window.hpp"

namespace cgvkp {
namespace data {
    class world;
}
namespace rendering {

    class debug_renderer {
    public:
        static const float presentation_scale;

        debug_renderer(const ::cgvkp::data::world& data);
        ~debug_renderer();

        bool init(const window& wnd);
        void render(const window& wnd);
        void deinit();

    private:

        // some utility functions for simple und INEFFICIENT rendering
        inline void set_color(float r, float g, float b, float a = 1.0f);
        inline void set_p1(float x, float y);
        inline void set_p2(float x, float y);
        inline void draw_line();
        inline void draw_line(float x1, float y1, float x2, float y2) {
            set_p1(x1, y1);
            set_p2(x2, y2);
            draw_line();
        }

        const ::cgvkp::data::world& data;

        unsigned int vao;
        unsigned int shader;
        unsigned int win_w, win_h;
    };

}
}
