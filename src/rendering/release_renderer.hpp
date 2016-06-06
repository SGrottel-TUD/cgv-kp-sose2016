#pragma once
#include "abstract_renderer.hpp"
#include "technique_normal_as_colour.hpp"
#include "model/model_base.hpp"
#include "view/view_base.hpp"
#include "controller/controller_base.hpp"
#include "glm/mat4x4.hpp"
#include <vector>
#include <chrono>

namespace cgvkp {
namespace data {
    class world;
}
namespace rendering {
	enum camera_mode {
		mono = 0,
		stereo = 1
	};

    class release_renderer : public abstract_renderer {
    public:
        release_renderer(const ::cgvkp::data::world& data);
        virtual ~release_renderer();
        virtual void render(const window& wnd);

		void set_camera_mode(camera_mode mode);
		void set_stereo_parameters(float eye_separation, float zzero_parallax);
		virtual void set_framebuffer_size(int width, int height);

		virtual void lost_context();
		virtual bool restore_context(window const& wnd);

        void add_model(model::model_base::ptr model);
        void remove_model(model::model_base::ptr model);
        void add_view(view::view_base::ptr view);
        void add_controller(controller::controller_base::ptr controller);

    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();

    private:
        bool has_context = false;

		void calculateProjection();
		void renderScene(glm::mat4x4 const& projection) const;

		glm::mat4 view;
		glm::mat4 leftProjection;	// Holds the projection matrix in mono mode.
		glm::mat4 rightProjection;
		camera_mode cameraMode;
		int framebufferWidth;
		int framebufferHeight;
		float eyeSeparation;
		float zZeroParallax;

		technique_normal_as_colour exampleTechnique;
		unsigned int vao;

		unsigned int vertexbuffer;
		float g_vertex_buffer_data[2 * 3 * 3];

        std::chrono::high_resolution_clock::time_point last_time;
        std::vector<model::model_base::ptr> models;
        std::vector<view::view_base::ptr> views;
        std::vector<view::view_base::ptr> cached_views;

        std::vector<controller::controller_base::ptr> controllers, new_controllers;
    };
}
}