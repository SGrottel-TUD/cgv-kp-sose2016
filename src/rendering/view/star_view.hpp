#pragma once
#include "rendering/model/star_model.hpp"
#include "rendering/view/star_geometry.hpp"
#include "rendering/view/view_geometry_base.hpp"
namespace cgvkp {
namespace rendering {
namespace view {
    class star_view : public view_geometry_base<star_geometry> {
    public:
        star_view();
        virtual ~star_view();
        virtual void render();
        std::shared_ptr<model::star_model> get_model() const;
    protected:
        virtual inline std::vector<std::string> techniques() { return _techniques; }
    private:
        static std::vector<std::string> _techniques;
    };
}
}
}