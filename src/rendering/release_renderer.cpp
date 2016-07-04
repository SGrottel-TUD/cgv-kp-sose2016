#include "release_renderer.hpp"
#include "data/world.hpp"
#include "glm/gtx/transform.hpp"
#include "view/star_view.hpp"
#include "view/hand_view.hpp"
#include <iostream>
#include "controller/data_controller.hpp"
#include "controller/cloud_controller.hpp"

cgvkp::rendering::release_renderer::release_renderer(::cgvkp::data::world const& data)
	: cgvkp::rendering::abstract_renderer(data),
	framebufferWidth(0), framebufferHeight(0), cameraMode(mono), fps_counter_elapsed(0.0), rendered_frames(0u)
{
	// Create and add data, cloud controller
	controllers.push_back(std::make_shared<controller::data_controller>(this, data));
	controllers.push_back(std::make_shared<controller::cloud_controller>(this, data));

	// Create a cached hand view to avoid delay on first hand appearance.
	cached_views.push_back(std::make_shared<view::hand_view>());

	ambientLight = glm::vec3(0.1, 0.1, 0.5) * 0.25f;
	directionalLight.color = glm::vec3(1, 1, 1);
	directionalLight.diffuseIntensity = 0.5f;
	directionalLight.direction = glm::normalize(glm::vec3(0, -2, -1));	// in view space.
}

bool cgvkp::rendering::release_renderer::init_impl(window const& wnd)
{
	wnd.make_current();

	if (!gbuffer.init() ||
		!postProcessing.init(2) ||
		!geometryPass.init() ||
		!directionalLightPass.init() ||
		!ssaoPass.init() ||
		!gaussianBlur.init() ||
		!quad.init("meshes/quad.obj") ||
		!background.init()
		)
	{
		return false;
	}

	for (auto const& view : cached_views)
	{
		if (!view->init())
		{
			return false;
		}
	}
	for (auto const& view : views)
	{
		if (!view->init())
		{
			return false;
		}
	}

	return true;
}

void cgvkp::rendering::release_renderer::deinit_impl()
{
	glBindVertexArray(0);
	glUseProgram(0);

	for (auto const& view : views)
	{
		view->deinit();
	}
	for (auto const& view : cached_views)
	{
		view->deinit();
	}

	background.deinit();
	quad.deinit();
	gaussianBlur.deinit();
	ssaoPass.deinit();
	directionalLightPass.deinit();
	geometryPass.deinit();
	gbuffer.deinit();
	postProcessing.deinit();
}

void cgvkp::rendering::release_renderer::calculateViewProjection()
{
	float fovy = glm::quarter_pi<float>();
	float tanHalfFovy = tan(fovy / 2);

	aspect = (cameraMode == stereo ? framebufferWidth / 2.0f : static_cast<float>(framebufferWidth)) / framebufferHeight;

	// View
	float w = data.get_config().width();
	float h = data.get_config().height();
	float k = 1.0f / 3;

	distance = (w + 0.5f) / (2 * aspect * tanHalfFovy);	// Distance to the front of the game area. 0.5f is a bias.

	float a = 2 * k * distance * sin(fovy / 2);	// projected h
	float cotTau = (1 - 2 * k) * tanHalfFovy;
	float tau = atan(1 / cotTau);	// tau: angle at a in direction of h
	float epsilon = a < h ? asin(a / h * sin(tau)) : tau;	// epsilon: angle at h in direction of a
	float beta = fovy / 2 + tau + epsilon - glm::half_pi<float>();	// beta: angle used for projection

	glm::vec3 position(0, sin(beta) * distance, cos(beta) * distance);

	glm::vec4 temp = glm::rotate(fovy / 2, glm::vec3(1, 0, 0)) * glm::vec4(-position, 1);
	glm::vec3 lookAt(temp.x, temp.y, temp.z);
	lookAt += position;

	position.x = w / 2;
	lookAt.x = w / 2;

	viewMatrix = glm::lookAt(position, lookAt, glm::vec3(0, 1, 0));

	// Projection
	float zNear = distance * cos(fovy / 2) - 1;	// Estimated minimum depth.
	if (zNear < 0.01f)
	{
		zNear = 0.01f;
	}
	float zFar = distance + h + 2;	// Estimated maximum depth.
	if (cameraMode == stereo)
	{
		glm::vec3 translation = glm::vec3(eyeSeparation * tanHalfFovy * zZeroParallax * aspect, 0.0f, 0.0f);

		float top = zNear * tanHalfFovy;
		float bottom = -top;

		float delta = -eyeSeparation * zNear * tanHalfFovy * aspect;
		float left = bottom * aspect - delta;
		float right = top * aspect - delta;
		leftProjection = glm::frustum(left, right, bottom, top, zNear, zFar) * glm::translate(translation);

		delta *= -1;
		left = bottom * aspect - delta;
		right = top * aspect - delta;
		rightProjection = glm::frustum(left, right, bottom, top, zNear, zFar) * glm::translate(-translation);
	}
	else	// mono
	{
		leftProjection = glm::perspective(fovy, aspect, zNear, zFar);
	}
}

void cgvkp::rendering::release_renderer::render(window const& wnd)
{
	wnd.make_current();
	if (wnd.get_size(framebufferWidth, framebufferHeight))
	{
		if (framebufferWidth != 0 && framebufferHeight != 0)
		{
			gbuffer.resize(cameraMode == mono ? framebufferWidth : framebufferWidth / 2, framebufferHeight);
			postProcessing.resize(cameraMode == mono ? framebufferWidth : framebufferWidth / 2, framebufferHeight);
			calculateViewProjection();
		}
	}
	if (framebufferWidth == 0 || framebufferHeight == 0)
	{
		return;
	}

	std::chrono::high_resolution_clock::time_point now_time = std::chrono::high_resolution_clock::now();
	double elapsed = std::chrono::duration<double>(now_time - last_time).count();
	last_time = now_time;
#if defined(DEBUG) || defined(_DEBUG)
	fps_counter_elapsed += elapsed;
	++rendered_frames;
	if (fps_counter_elapsed >= 2.0)
	{
		std::cout << "FPS: " << rendered_frames / fps_counter_elapsed << std::endl;
		fps_counter_elapsed = 0.0;
		rendered_frames = 0u;
	}
#endif

	if (new_controllers.size())
	{
		controllers.insert(controllers.end(), new_controllers.begin(), new_controllers.end());
		new_controllers.clear();
	}
	// Remove views and controllers without model
	for (auto it = views.begin(); it != views.end();)
	{
		if (!(*it)->has_model())
		{
			it->get()->deinit();
			it = views.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto it = controllers.begin(); it != controllers.end();)
	{
		if (!(*it)->has_model())
		{
			it = controllers.erase(it);
		}
		else
		{
			(*it)->update(elapsed, wnd.get_user_input_object());
			++it;
		}
	}

	if (cameraMode == stereo)
	{
		glViewport(0, 0, framebufferWidth / 2, framebufferHeight);

		// Render left image.
		renderScene(leftProjection);
		gbuffer.bindForReadingFinal();
		glBlitFramebuffer(0, 0, framebufferWidth / 2, framebufferHeight, 0, 0, framebufferWidth / 2, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// Render right image.
		renderScene(rightProjection);
		gbuffer.bindForReadingFinal();
		glBlitFramebuffer(0, 0, framebufferWidth / 2, framebufferHeight, framebufferWidth / 2, 0, framebufferWidth, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	else	// mono
	{
		glViewport(0, 0, framebufferWidth, framebufferHeight);
		renderScene(leftProjection);
		gbuffer.bindForReadingFinal();
		glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, framebufferWidth, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
}

void cgvkp::rendering::release_renderer::renderScene(glm::mat4 const& projection) const
{
	gbuffer.bindForWritingGeometry();
	geometryPass.use();

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render a quad with a very low z value in view space to not get wrong results during the ambient occlusion.
	geometryPass.setWorldView(glm::translate(glm::vec3(0, 0, -1000)));
	geometryPass.setWorldViewProjection(glm::mat4(1));
	quad.render();

	glEnable(GL_DEPTH_TEST);

	for (view::view_base::ptr v : views)
	{
		geometryPass.renderView(v, viewMatrix, projection);
	}

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	// Ambient light
	gbuffer.bindForReadingGeometry();
	postProcessing.nextPass(0, 0);
	ssaoPass.use();
	ssaoPass.setAmbientLight(ambientLight);
	quad.render();

	postProcessing.nextPass(1);
	gaussianBlur.use();
	gaussianBlur.setDirection(GaussianBlurTechnique::horizontal);
	gaussianBlur.setBlurSize(framebufferWidth);
	quad.render();

	postProcessing.finalPass(1);
	gbuffer.bindForWritingFinal();
	gaussianBlur.setDirection(GaussianBlurTechnique::vertical);
	gaussianBlur.setBlurSize(framebufferHeight);
	quad.render();

	// Directional Light
	gbuffer.bindForReadingGeometry();
	gbuffer.bindForWritingFinal();
	directionalLightPass.use();
	directionalLightPass.setLight(directionalLight);

	glEnable(GL_BLEND);
	quad.render();
	glDisable(GL_BLEND);

	// Background
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_EQUAL);
	background.render();
	quad.render();
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
}

void cgvkp::rendering::release_renderer::setCameraMode(CameraMode mode)
{
	cameraMode = mode;
	framebufferWidth = 0;	// Reset framebuffer width. The test in the render method will do the rest.
}

void cgvkp::rendering::release_renderer::setStereoParameters(float _eyeSeparation, float _zZeroParallax)
{
	eyeSeparation = _eyeSeparation;
	zZeroParallax = _zZeroParallax;
	if (framebufferWidth != 0 && framebufferHeight != 0)
	{
		calculateViewProjection();
	}
}
