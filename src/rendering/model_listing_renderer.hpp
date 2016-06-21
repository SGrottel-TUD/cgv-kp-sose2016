#pragma once
#include "rendering/abstract_renderer.hpp"
#include "rendering/model/star_model.hpp"
#include "rendering/view/star_view.hpp"
#include "rendering/model/hand_model.hpp"
#include "rendering/view/hand_view.hpp"
#include "rendering/technique/technique_normal_as_colour.hpp"
#include "rendering/technique/background_technique.hpp"
#include <glm/glm.hpp>
#include <chrono>

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
    private:
        technique_normal_as_colour technique1;
        background_technique bg;
		std::chrono::high_resolution_clock::time_point last_time;
        std::shared_ptr<model::star_model> star_model;
        std::shared_ptr<view::star_view> star_view;
        std::shared_ptr<model::hand_model> hand_model;
        std::shared_ptr<view::hand_view> hand_view;
    };
}
}