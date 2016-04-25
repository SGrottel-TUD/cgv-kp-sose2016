#pragma once
#include "rendering/abstract_renderer.hpp"

namespace cgvkp {
namespace data {
    class world;
}
namespace rendering {
    class release_renderer : public abstract_renderer {
    public:
        release_renderer(const ::cgvkp::data::world& data);
        virtual ~release_renderer();
        virtual void render(const window& wnd);

    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();
    };
}
}