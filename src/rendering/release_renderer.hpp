#pragma once

#include <chrono>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <list>
#include <unordered_map>
#include "abstract_renderer.hpp"
#include "geometryBuffer.hpp"
#include "postProcessingFramebuffer.hpp"
#include "technique/geometry.hpp"
#include "technique/directionalLight.hpp"
#include "technique/spotLight.hpp"
#include "technique/ssao.hpp"
#include "technique/gaussianBlur.hpp"
#include "technique/background_technique.hpp"
#include "technique/star.hpp"
#include "technique/spriteGeometry.hpp"
#include "lights.hpp"
#include "mesh.hpp"
#include "model/model_base.hpp"
#include "view/cloud_view.hpp"
#include "view/hand_view.hpp"
#include "view/star_view.hpp"
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
			enum CameraMode
			{
				mono = 0,
				stereo = 1
			};

			release_renderer(data::world & data, window& wnd);
			virtual void render(window const& wnd);

			void setCameraMode(CameraMode mode);
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

			// Rendering steps
			void fillGeometryBuffer(glm::mat4 const& projection) const;
			void addAmbientLight() const;
			void addDirectionalLight(DirectionalLight const& light) const;
			void setBackground() const;
			void addStarLights(glm::mat4 const& projection) const;
			void addStars(glm::mat4 const& projection) const;

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
			CameraMode cameraMode;
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
			background_technique background;
			StarTechnique starPass;
			SpriteGeometryTechnique spriteGeometryPass;
			Mesh const* pQuad;
			Mesh const* pPyramid;

			glm::vec3 ambientLight;
			DirectionalLight directionalLight;

			std::chrono::high_resolution_clock::duration dt;
			std::chrono::high_resolution_clock::time_point tGame;
			std::chrono::high_resolution_clock::time_point tLogic;
			unsigned int frames;
			int fps;

			std::unordered_map<char const*, Mesh> meshes;

			std::list<model::model_base::ptr> models;
			std::list<view::cloud_view::ptr> cloudViews;
			std::list<view::hand_view::ptr> handViews;
			std::list<view::star_view::ptr> starViews;

			std::list<controller::controller_base::ptr> controllers;
			std::shared_ptr<controller::CloudController> cloudController;

			Gui gui;
		};
	}
}
