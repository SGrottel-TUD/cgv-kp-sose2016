#pragma once
#include "rendering/view/view_base.hpp"
#include <memory>

namespace cgvkp {
namespace rendering {
namespace view {

    template<class geo_type>
    class view_geometry_base : public view_base {
    public:

        view_geometry_base() : view_base(), geo() {
            // intentionally empty
        }
        virtual ~view_geometry_base() {
            // intentionally empty
        }

    protected:

        virtual bool init_impl();
        virtual void deinit_impl();

        std::shared_ptr<geo_type> geo;

    private:

        static std::weak_ptr<geo_type> geo_cache;

    };

    template<class geo_type>
    bool view_geometry_base<geo_type>::init_impl() {
        std::shared_ptr<geo_type> g = geo_cache.lock();

        if (!g) {
            g = std::make_shared<geo_type>();
            if (!g->init()) return false;
            geo_cache = g;
        }
        geo = g;

        return true;
    }

    template<class geo_type>
    void view_geometry_base<geo_type>::deinit_impl() {
        geo.reset();
    }

    template<class geo_type>
    std::weak_ptr<geo_type> view_geometry_base<geo_type>::geo_cache;

}
}
}
