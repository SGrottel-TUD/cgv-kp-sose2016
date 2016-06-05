#pragma once

#include <chrono>
#include <glm/mat4x4.hpp>
#include <list>
#include <vector>
#include "abstract_renderer.hpp"
#include "geometryBuffer.hpp"
#include "lights.hpp"
#include "technique.hpp"
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

    protected:
        virtual bool init_impl(const window& wnd);
        virtual void deinit_impl();

    private:
		void calculateProjection();
		void renderScene(glm::mat4x4 const& projection) const;

		glm::mat4 view;
		glm::mat4 leftProjection;	// Holds the projection matrix in mono mode.
		glm::mat4 rightProjection;
		camera_mode cameraMode;
		GLsizei framebufferWidth;
		GLsizei framebufferHeight;
		float eyeSeparation;
		float zZeroParallax;

		GLuint areaVao;
		GLuint sphereVao;

		GLuint vertexBuffer;
		GLuint indexBuffer;
		GLuint sphereVertexBuffer;
		GLuint sphereIndexBuffer;

		std::list<PointLight> pointLights;
		GeometryBuffer gbuffer;
		GeometryTechnique geometryPass;
		ShadowVolumeTechnique shadowVolumePass;
		LightTechnique lightPass;

        std::chrono::high_resolution_clock::time_point last_time;
        std::vector<model::model_base::ptr> models;
        std::vector<view::view_base::ptr> views;
        std::vector<controller::controller_base::ptr> controllers;
    };
}
}