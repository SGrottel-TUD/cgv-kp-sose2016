#pragma once
#include "rendering/window.hpp"

namespace cgvkp {
namespace data {
    class world;
}
namespace rendering {

    class abstract_renderer {
    public:
        abstract_renderer(const ::cgvkp::data::world& data);
        virtual ~abstract_renderer();
        // Initializes the object (if not already initialized) and returns the initialization state
        bool init(const window& wnd);
        // deinitializes the object (if initialized)
        void deinit();

        // Do render
        virtual void render(const window& wnd) = 0;

		virtual void set_framebuffer_size(int width, int height) = 0;

		virtual void lost_context() {}
		virtual bool restore_context(window const& wnd) { return true; }

    protected:
        // return true on success
        virtual bool init_impl(const window& wnd) = 0;
        virtual void deinit_impl() = 0;

        const ::cgvkp::data::world& data;
    private:
        bool initialized;
    };

}
}
