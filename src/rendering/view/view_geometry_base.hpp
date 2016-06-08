#pragma once
#include "rendering/view/view_base.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>

namespace cgvkp {
namespace rendering {
namespace view {

	enum VertexAttributeLocation
	{
		position = 0,
		normal,
		textureCoord,
		numAttributes
	};

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

        static std::map<GLFWwindow*, std::weak_ptr<geo_type> > geo_cache;

    };

    template<class geo_type>
    bool view_geometry_base<geo_type>::init_impl() {
        std::shared_ptr<geo_type> g;
        auto context = glfwGetCurrentContext();
        if (geo_cache.count(context) > 0) {
            g = geo_cache[context].lock();
        }

        if (!g) {
            g = std::make_shared<geo_type>();
            if (!g->init()) return false;
            if (geo_cache.count(context) > 0) {
                geo_cache.erase(context);
            }
            geo_cache.insert(std::make_pair(context, g));
        }
        geo = g;

        return true;
    }

    template<class geo_type>
    void view_geometry_base<geo_type>::deinit_impl() {
        geo.reset();
    }

    template<class geo_type>
    std::map<GLFWwindow*, std::weak_ptr<geo_type> > view_geometry_base<geo_type>::geo_cache;

}
}
}
