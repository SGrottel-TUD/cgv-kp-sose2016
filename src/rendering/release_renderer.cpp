#include <glm/gtx/transform.hpp>
#include <iostream>
#include "release_renderer.hpp"
#include "data/world.hpp"
#include "controller/data_controller.hpp"
#include "controller/cloudController.hpp"
#include "window.hpp"

cgvkp::rendering::release_renderer::release_renderer(::cgvkp::data::world & data, window& wnd)
	: abstract_renderer(data), windowWidth(0), windowHeight(0), framebufferWidth(0), framebufferHeight(0), cameraMode(mono), gui(data, "CartoonRegular.ttf"), frames(0)
{
	Mesh const& quadMesh = meshes.insert({ "quad", Mesh() }).first->second;
	Mesh const& cloudMesh = meshes.insert({ "cloudsprite", Mesh() }).first->second;
	Mesh const& starMesh = meshes.insert({ "star", Mesh() }).first->second;
	Mesh const& handMesh = meshes.insert({ "hand", Mesh() }).first->second;
	pPyramid = &meshes.insert({ "pyramid", Mesh() }).first->second;

	pQuad = &quadMesh;

	// Create and add data, cloud controller
	dt = std::chrono::microseconds(1000000 / 60);
	controllers.push_back(std::make_shared<controller::data_controller>(this, data, handMesh, starMesh));
	controllers.push_back(cloudController = std::make_shared<controller::CloudController>(this, data, cloudMesh));

	// Lights
	ambientLight = glm::vec3(0.1, 0.1, 0.5);
	directionalLight.color = glm::vec3(1, 1, 1);
	directionalLight.diffuseIntensity = 0.5f;
	directionalLight.direction = glm::normalize(glm::vec3(0, -2, -1));	// in view space.

	// Gui
	wnd.setMousePositionCallback([&](double x, double y) { gui.updateMousePosition(static_cast<float>(x), static_cast<float>(y)); });
	wnd.setLeftMouseButtonCallback(std::bind(&Gui::click, &gui));
	wnd.setInputCodePointCallback(std::bind(&Gui::inputCodePoint, &gui, std::placeholders::_1));
	gui.setExitCallback(std::bind(&window::close, &wnd));
	gui.setHighscoresCallback([]() { return std::list<Score>(); });
	gui.setScoreCallback(std::bind(&data::world::get_score, &data));
	gui.setFPSCallback([&]() { return fps; });
	gui.loadMenu();
}

bool cgvkp::rendering::release_renderer::init_impl(window const& wnd)
{
	wnd.make_current();

	if (!gbuffer.init() ||
		!postProcessing.init(2) ||
		!geometryPass.init() ||
		!directionalLightPass.init() ||
		!spotLightPass.init() ||
		!ssaoPass.init() ||
		!gaussianBlur.init() ||
		!background.init() ||
		!starPass.init() ||
		!spriteGeometryPass.init() ||
		!gui.init())
	{
		return false;
	}

	for (auto& pair : meshes)
	{
		pair.second.init(pair.first);
	}

	tGame = tLogic = std::chrono::high_resolution_clock::now();

	return true;
}

void cgvkp::rendering::release_renderer::deinit_impl()
{
	glBindVertexArray(0);
	glUseProgram(0);

	for (auto& pair : meshes)
	{
		pair.second.deinit();
	}

	background.deinit();
	gaussianBlur.deinit();
	ssaoPass.deinit();
	directionalLightPass.deinit();
	spotLightPass.deinit();
	geometryPass.deinit();
	gbuffer.deinit();
	postProcessing.deinit();
	starPass.deinit();
	spriteGeometryPass.deinit();
	gui.deinit();
}

void cgvkp::rendering::release_renderer::setCameraMode(CameraMode mode)
{
	cameraMode = mode;
	windowWidth = 0;	// Reset window width. The test in the render method will do the rest.
}

void cgvkp::rendering::release_renderer::setStereoParameters(float _eyeSeparation, float _zZeroParallax)
{
	eyeSeparation = _eyeSeparation;
	zZeroParallax = _zZeroParallax;
	if (zZeroParallax < 0.01f)	// zNear
	{
		zZeroParallax = 0.01f;
	}
	if (framebufferWidth != 0 && framebufferHeight != 0)
	{
		calculateViewProjection();
		gui.setSize(framebufferWidth, framebufferHeight, glm::quarter_pi<float>(), cameraMode == stereo ? zZeroParallax : 10);
	}
}

void cgvkp::rendering::release_renderer::calculateViewProjection()
{
	if (framebufferWidth == 0 || framebufferHeight == 0)
	{
		return;
	}

	float fovy = glm::quarter_pi<float>();
	float tanHalfFovy = tan(fovy / 2);

	float aspect = static_cast<float>(framebufferWidth) / framebufferHeight;

	// View
	float w = data.get_config().width();
	float h = data.get_config().height();
	float k = 1.0f / 2;

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
	float zNear = 0.01f;
	if (zNear < 0.01f)
	{
		zNear = 0.01f;
	}
	float zFar = distance + h + 2;	// Estimated maximum depth.
	zFar = 100;
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

void cgvkp::rendering::release_renderer::update(double seconds, std::shared_ptr<abstract_user_input> const& input)
{
	// Remove views and controllers without model
	for (auto it = cloudViews.begin(); it != cloudViews.end();)
	{
		if (!(*it)->has_model())
		{
			it = cloudViews.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto it = handViews.begin(); it != handViews.end();)
	{
		if (!(*it)->has_model())
		{
			it = handViews.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto it = starViews.begin(); it != starViews.end();)
	{
		if (!(*it)->has_model())
		{
			it = starViews.erase(it);
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
			(*it)->update(seconds, input);
			++it;
		}
	}
}

void cgvkp::rendering::release_renderer::render(window const& wnd)
{
	wnd.make_current();
	if (wnd.get_size(windowWidth, windowHeight))
	{
		framebufferWidth = cameraMode == stereo ? windowWidth / 2 : windowWidth;
		framebufferHeight = windowHeight;
		if (framebufferWidth != 0 && framebufferHeight != 0)
		{
			gbuffer.resize(framebufferWidth, framebufferHeight);
			postProcessing.resize(framebufferWidth, framebufferHeight);
            gui.setSize(framebufferWidth, framebufferHeight, glm::quarter_pi<float>(), cameraMode == stereo ? zZeroParallax : 10);
			calculateViewProjection();
		}
	}
	if (framebufferWidth == 0 || framebufferHeight == 0)
	{
		return;
	}

	++frames;
	auto tNow = std::chrono::high_resolution_clock::now();
	auto dtLogic = tNow - tLogic;
	if (dtLogic >= dt)
	{
		update(static_cast<double>(dtLogic.count()) / 1000000000, wnd.get_user_input_object());
		tLogic = tNow;
	}
	if (tNow - tGame >= std::chrono::milliseconds(500))
	{
		fps = static_cast<int>(2 * frames);
		frames = 0;
		tGame = tNow;
	}

	glViewport(0, 0, framebufferWidth, framebufferHeight);

	// Render left image in stereo or single image in mono.
	renderScene(leftProjection);
	gbuffer.bindForReadingFinal();
	glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, framebufferWidth, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	if (cameraMode == stereo)
	{
		// Render right image.
		renderScene(rightProjection);
		gbuffer.bindForReadingFinal();
		glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, framebufferWidth, 0, 2 * framebufferWidth, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
}

void cgvkp::rendering::release_renderer::renderScene(glm::mat4 const& projection) const
{
	fillGeometryBuffer(projection);

	glBlendEquation(GL_FUNC_ADD);

	setBackground();
	addAmbientLight();
	addDirectionalLight(directionalLight);
	addStarLights(projection);
	addStars(projection);
	gbuffer.bindForWritingFinal();
	gui.render(projection);
}

void cgvkp::rendering::release_renderer::fillGeometryBuffer(glm::mat4 const& projection) const
{
	gbuffer.bindForWritingGeometry();
	geometryPass.use();

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the background with a very low z value in view space to not get wrong results during post processing.
	geometryPass.setWorldView(glm::translate(glm::vec3(0, 0, -10)));
	geometryPass.setWorldViewProjection(glm::mat4(1));
	background.render();
	pQuad->render();

	glEnable(GL_DEPTH_TEST);
	for (auto const& v : handViews)
	{
		auto graphic_model = v->get_model();
		if (graphic_model == nullptr) continue;
		geometryPass.setWorldView(viewMatrix * graphic_model->model_matrix);
		geometryPass.setWorldViewProjection(projection * viewMatrix * graphic_model->model_matrix);
		v->render();
	}

	// Render clouds as point sprites.
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	spriteGeometryPass.use();

	for (auto const& v : cloudViews)
	{
		auto graphic_model = v->get_model();
		if (graphic_model == nullptr) continue;
		glm::mat4 worldView = viewMatrix * graphic_model->model_matrix;
		spriteGeometryPass.setWorldView(worldView);
		spriteGeometryPass.setWorldViewProjection(projection * worldView);
		v->render();
	}
}

void cgvkp::rendering::release_renderer::addAmbientLight() const
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gbuffer.bindForReadingGeometry();
	postProcessing.nextPass(0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	ssaoPass.use();
	ssaoPass.setAmbientLight(ambientLight);
	pQuad->render();

	postProcessing.nextPass(1);
	gaussianBlur.use();
	gaussianBlur.setDirection(GaussianBlurTechnique::horizontal);
	gaussianBlur.setBlurSize(framebufferWidth);
	pQuad->render();

	postProcessing.finalPass(1);
	gbuffer.bindForWritingFinal();
	gaussianBlur.setDirection(GaussianBlurTechnique::vertical);
	gaussianBlur.setBlurSize(framebufferHeight);
	glEnable(GL_BLEND);
	pQuad->render();
}

void cgvkp::rendering::release_renderer::addDirectionalLight(DirectionalLight const& light) const
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gbuffer.bindForReadingGeometry();
	gbuffer.bindForWritingFinal();
	directionalLightPass.use();

	directionalLightPass.setLight(directionalLight);
	pQuad->render();
}

void cgvkp::rendering::release_renderer::setBackground() const
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	gbuffer.bindForWritingFinal();
	background.use();

	background.setScreenSize(framebufferWidth, framebufferHeight);
	background.render();
	pQuad->render();
}

void cgvkp::rendering::release_renderer::addStarLights(glm::mat4 const& projection) const
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	gbuffer.bindForReadingGeometry();
	gbuffer.bindForWritingFinal();
	spotLightPass.use();

	SpotLight starLight;
	starLight.color = glm::vec3(0.9608f, 0.5059f, 0.1255f);	// average color in the star texture.
	starLight.diffuseIntensity = 1;
	starLight.direction = glm::vec3(viewMatrix * glm::vec4(0, -1, 0, 0));	// in view space
	starLight.attenuation = glm::vec3(1, 0, 2);	// constant, linear, exponential
	starLight.cutoff = glm::quarter_pi<float>();

	glm::vec3 scale;
	scale.y = starLight.calculateMaxDistance();
	scale.x = scale.z = scale.y * tan(starLight.cutoff);

	for (auto const& s : starViews)
	{
		if (!s->has_model()) continue;

		starLight.position = glm::vec3(viewMatrix * s->get_model()->model_matrix[3]);	// in view space
		spotLightPass.setLight(starLight);

		glm::mat4 trans = glm::translate(glm::vec3(s->get_model()->model_matrix[3]));
		glm::mat4 world = glm::scale(trans, scale);
		spotLightPass.setWorldViewProjection(projection * viewMatrix * world);
		pPyramid->render();
	}
}

void cgvkp::rendering::release_renderer::addStars(glm::mat4 const& projection) const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	gbuffer.bindForWritingFinal();
	starPass.use();

	float t = static_cast<float>(glfwGetTime()) / 8;
	for (auto const& s : starViews)
	{
		auto model = s->get_model();
		if (model == nullptr) continue;

		glm::vec2 offset = glm::vec2(cos(model->id), sin(model->id)) * t;
		offset.x -= static_cast<long>(offset.x);
		offset.y -= static_cast<long>(offset.y);
		starPass.setTextureCoordOffset(offset);
		starPass.setWorldViewProjection(projection * viewMatrix * model->model_matrix);
		s->render();
	}
}
