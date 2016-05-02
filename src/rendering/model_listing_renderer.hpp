#pragma once
#include "rendering/abstract_renderer.hpp"
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
		unsigned int vao;
		unsigned int shader;
		unsigned int buffers[3];
		unsigned int element_count;
		unsigned int model_loc, projection_loc, view_loc;
		glm::mat4 model_matrix, projection_matrix, view_matrix;
		std::chrono::high_resolution_clock::time_point last_time;
    };
}
}