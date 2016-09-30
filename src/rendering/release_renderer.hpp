#pragma once

#include <chrono>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <list>
#include "application_config.hpp"
#include "abstract_renderer.hpp"
#include "geometryBuffer.hpp"
#include "postProcessingFramebuffer.hpp"
#include "technique/background.hpp"
#include "technique/default.hpp"
#include "technique/directionalLight.hpp"
#include "technique/gaussianBlur.hpp"
#include "technique/geometry.hpp"
#include "technique/spotLight.hpp"
#include "technique/spriteGeometry.hpp"
#include "technique/ssao.hpp"
#include "lights.hpp"
#include "mesh.hpp"
#include "model/model_base.hpp"
#include "view/cloud_view.hpp"
#include "view/hand_view.hpp"
#include "view/star_view.hpp"
#include "view/moon_view.hpp"
#include "controller/controller_base.hpp"
#include "controller/cloudController.hpp"
#include "gui.hpp"

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
			release_renderer(application_config& config, data::world& data, window& wnd);
			virtual void render(window const& wnd);

			void setCameraMode(application_config::CameraMode mode);
			void setStereoParameters(float eyeSeparation, float zZeroParallax);

			inline float getDistance() const { return distance; }
			inline float getAspect() const { return static_cast<float>(framebufferWidth) / framebufferHeight; }

			inline void add_model(model::model_base::ptr const& model) { models.push_back(model); }
			inline void remove_model(model::model_base::ptr const& model) { models.remove(model); }
			inline void add_view(view::cloud_view::ptr const& v) { cloudViews.push_back(v); }
			inline void add_view(view::hand_view::ptr const& v) { handViews.push_back(v); }
			inline void add_view(view::star_view::ptr const& v) { starViews.push_back(v); }
			inline void add_controller(controller::controller_base::ptr const& controller) { controllers.push_back(controller); }

		protected:
			virtual bool init_impl(window const& wnd);
			virtual void deinit_impl();

		private:
			void calculateViewProjection();
			void renderScene(glm::mat4 const& projection) const;
			void update(double seconds, std::shared_ptr<abstract_user_input> const& input);
			void reload();

			// Rendering steps
			void fillGeometryBuffer(glm::mat4 const& projection) const;
			void addAmbientLight() const;
			void addDirectionalLight(DirectionalLight const& light) const;
			void addStarLights(glm::mat4 const& projection) const;
			void addStarsAndMoon(glm::mat4 const& projection) const;

			window& wnd;
			GLsizei windowWidth;
			GLsizei windowHeight;
			GLsizei framebufferWidth;
			GLsizei framebufferHeight;
			float fovy;
			float zNear;
			float zFar;

			glm::mat4 viewMatrix;
			glm::mat4 leftProjection;	// Holds the projection matrix in mono mode.
			glm::mat4 rightProjection;
			application_config::CameraMode cameraMode;
			float eyeSeparation;
			float zZeroParallax;

			float distance;

			GeometryBuffer gbuffer;
			PostProcessingFramebuffer postProcessing;
			GeometryTechnique geometryPass;
			DirectionalLightTechnique directionalLightPass;
			SpotLightTechnique spotLightPass;
			SSAOTechnique ssaoPass;
			GaussianBlurTechnique gaussianBlur;
			DefaultTechnique defaultPass;
			SpriteGeometryTechnique spriteGeometryPass;
			BackgroundTechnique backgroundPass;

			glm::vec3 ambientLight;
			DirectionalLight directionalLight;

			std::chrono::high_resolution_clock::duration dt;
			std::chrono::high_resolution_clock::time_point tGame;
			std::chrono::high_resolution_clock::time_point tLogic;
			unsigned int frames;
			int fps;

			enum Meshes
			{
				cloud = 0, hand, pyramid, space, star, moon, quad,
				numMeshes
			};
			std::vector<Mesh> meshes;

			std::list<model::model_base::ptr> models;
			std::vector<view::cloud_view::ptr> cloudViews;
			std::list<view::hand_view::ptr> handViews;
			std::list<view::star_view::ptr> starViews;
			view::moon_view::ptr moonView;

			std::list<controller::controller_base::ptr> controllers;
			std::shared_ptr<controller::CloudController> cloudController;

			Gui gui;
		};
	}
}
