#pragma once
#include "rendering/model/hand_model.hpp"
#include "rendering/view/hand_geometry.hpp"
#include "rendering/view/view_geometry_base.hpp"
namespace cgvkp {
namespace rendering {
namespace view {
    class hand_view : public view_geometry_base<hand_geometry> {
    public:
        hand_view();
        virtual ~hand_view();
        virtual void render();
        std::shared_ptr<model::hand_model> get_model() const;
    };
}
}
}