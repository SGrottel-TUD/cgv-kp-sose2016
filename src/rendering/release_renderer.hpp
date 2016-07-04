#pragma once

#include <chrono>
#include <glm/mat4x4.hpp>
#include <list>
#include "abstract_renderer.hpp"
#include "geometryBuffer.hpp"
#include "postProcessingFramebuffer.hpp"
#include "technique/geometry.hpp"
#include "technique/directionalLight.hpp"
#include "technique/ssao.hpp"
#include "technique/gaussianBlur.hpp"
#include "technique/background_technique.hpp"
#include "lights.hpp"
#include "mesh.hpp"
#include "model/model_base.hpp"
#include "view/view_base.hpp"
#include "controller/controller_base.hpp"

namespace cgvkp
{
	namespace data
	{
		class world;
	}
	namespace rendering
	{
		class release_renderer : public abstract_renderer
		{
		public:
			enum CameraMode
			{
				mono = 0,
				stereo = 1
			};

			release_renderer(data::world const& data);
			virtual void render(window const& wnd);

			void setCameraMode(CameraMode mode);
			void setStereoParameters(float eyeSeparation, float zZeroParallax);

			inline float getDistance() const { return distance; }
			inline float getAspect() const { return aspect; }

			inline void add_model(model::model_base::ptr const& model) { models.push_back(model); }
			inline void remove_model(model::model_base::ptr const& model) { models.remove(model); }
			inline void add_view(view::view_base::ptr const& view) { views.push_back(view); }
			inline void add_controller(controller::controller_base::ptr const& controller) { new_controllers.push_back(controller); }

		protected:
			virtual bool init_impl(window const& wnd);
			virtual void deinit_impl();

		private:

			void calculateViewProjection();
			void renderScene(glm::mat4 const& projection) const;

			glm::mat4 viewMatrix;
			glm::mat4 leftProjection;	// Holds the projection matrix in mono mode.
			glm::mat4 rightProjection;
			CameraMode cameraMode;
			GLsizei framebufferWidth;
			GLsizei framebufferHeight;
			float eyeSeparation;
			float zZeroParallax;

			float distance;
			float aspect;

			GeometryBuffer gbuffer;
			PostProcessingFramebuffer postProcessing;
			GeometryTechnique geometryPass;
			DirectionalLightTechnique directionalLightPass;
			SSAOTechnique ssaoPass;
			GaussianBlurTechnique gaussianBlur;
			background_technique background;
			Mesh quad;

			glm::vec3 ambientLight;
			DirectionalLight directionalLight;

			std::chrono::high_resolution_clock::time_point last_time;
			double fps_counter_elapsed;
			unsigned int rendered_frames;

			std::list<model::model_base::ptr> models;
			std::list<view::view_base::ptr> views;
			std::list<view::view_base::ptr> cached_views;

			std::list<controller::controller_base::ptr> controllers, new_controllers;
		};
	}
}
