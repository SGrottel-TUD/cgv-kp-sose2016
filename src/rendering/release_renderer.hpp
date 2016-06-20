#pragma once

#include <chrono>
#include <glm/mat4x4.hpp>
#include <list>
#include <vector>
#include "abstract_renderer.hpp"
#include "technique/geometryBuffer.hpp"
#include "technique/ShadowVolume.hpp"
#include "technique/LightTechnique.hpp"
#include "technique/background_technique.hpp"
#include "lights.hpp"
#include "mesh.hpp"
#include "model/model_base.hpp"
#include "view/view_base.hpp"
#include "controller/controller_base.hpp"

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

		double fps_last_time;
		int nbFrames;

        bool has_context = false;

		void calculateViewProjection();
		void renderScene(glm::mat4x4 const& projection) const;
        void renderLights(glm::mat4x4 const& projection) const;
        void renderPointLight(PointLight const& pointLight, glm::mat4x4 const& projection) const;

		glm::mat4 viewMatrix;
		glm::mat4 leftProjection;	// Holds the projection matrix in mono mode.
		glm::mat4 rightProjection;
		camera_mode cameraMode;
		GLsizei framebufferWidth;
		GLsizei framebufferHeight;
		float eyeSeparation;
		float zZeroParallax;

		std::list<PointLight> pointLights;
		GeometryBuffer gbuffer;
		GeometryTechnique geometryPass;
		ShadowVolumeTechnique shadowVolumePass;
		LightTechnique lightPass;
        background_technique background;
		Mesh lightingSphere;

        std::chrono::high_resolution_clock::time_point last_time;
        std::vector<model::model_base::ptr> models;
        std::vector<view::view_base::ptr> views;
        std::vector<view::view_base::ptr> cached_views;

        std::vector<controller::controller_base::ptr> controllers, new_controllers;
    };
}
}