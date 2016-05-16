#pragma once
#include "rendering/abstract_renderer.hpp"
#include "rendering/model/model_base.hpp"
#include "rendering/view/view_base.hpp"
#include "rendering/controller/controller_base.hpp"
#include <vector>
#include <chrono>

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

		virtual void set_framebuffer_size(int width, int height);

    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();
    private:
        std::chrono::high_resolution_clock::time_point last_time;
        std::vector<model::model_base::ptr> models;
        std::vector<view::view_base::ptr> views;
        std::vector<controller::controller_base::ptr> controllers;
    };
}
}