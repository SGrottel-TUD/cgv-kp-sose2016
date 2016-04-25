#pragma once
#include "rendering/abstract_renderer.hpp"

namespace cgvkp {
namespace data {
    class world;
}
namespace rendering {
    class model_listing_renderer : public abstract_renderer {
    public:
        model_listing_renderer(const ::cgvkp::data::world& data);
        virtual ~model_listing_renderer();
        virtual void render(const window& wnd);

    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();
    };
}
}