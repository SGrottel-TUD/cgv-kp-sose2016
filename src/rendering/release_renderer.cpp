#include "release_renderer.hpp"
#include "data/world.hpp"
#include "glm/gtx/transform.hpp"
#include "view/star_view.hpp"
#include "view/hand_view.hpp"
#include <iostream>
#include "controller/data_controller.hpp"
#include "controller/cloud_controller.hpp"

cgvkp::rendering::release_renderer::release_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
	framebufferWidth(0), framebufferHeight(0), cameraMode(mono), fps_last_time(glfwGetTime()), nbFrames(0) {
}
cgvkp::rendering::release_renderer::~release_renderer(){}

bool cgvkp::rendering::release_renderer::init_impl(const window& wnd) {
	if (!restore_context(wnd))
	{
		return false;
	}

    // Create and add data, cloud controller
    controllers.push_back(std::make_shared<controller::data_controller>(this, data));
	controllers.push_back(std::make_shared<controller::cloud_controller>(this, data));

    // Create a cached hand view to avoid delay on first hand appearance.
    auto hand_view = std::make_shared<view::hand_view>();
    hand_view->init();
    cached_views.push_back(hand_view);

	// light source
	PointLight p1;
	p1.position = glm::vec3(0, 1.5f, 0);
	p1.color = glm::vec3(1, 1, 0.6f);
	p1.ambientIntensity = 0.25f;
	p1.diffuseIntensity = 0.8f;
	p1.constantAttenuation = 0;
	p1.linearAttenuation = 0;
	p1.exponentialAttenuation = 0.3f;
	p1.calculateWorld();
	pointLights.push_back(p1);

	PointLight p2;
	p2.position = glm::vec3(0, 1.5f, -3);
	p2.color = glm::vec3(1, 1, 0.6f);
	p2.ambientIntensity = 0.25f;
	p2.diffuseIntensity = 0.2f;
	p2.constantAttenuation = 0;
	p2.linearAttenuation = 0;
	p2.exponentialAttenuation = 0.3f;
	p2.calculateWorld();
	pointLights.push_back(p2);

	return true;
}

void cgvkp::rendering::release_renderer::deinit_impl()
{
	lost_context();
}

void cgvkp::rendering::release_renderer::calculateViewProjection()
{
	float fovy = glm::quarter_pi<float>();

	float tanHalfFovy = tan(fovy / 2);
	aspect = static_cast<float>(framebufferWidth / (cameraMode == stereo ? 2 : 1)) / framebufferHeight;

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

void cgvkp::rendering::release_renderer::render(const window& wnd)
{

	double current_time = glfwGetTime();
	nbFrames++;
	if (current_time - fps_last_time >= 1.0) {
		printf("%i fps\n",  nbFrames);
		nbFrames = 0;
		fps_last_time += 1.0;
	}

	wnd.make_current();
	if (wnd.get_size(framebufferWidth, framebufferHeight))
	{
		gbuffer.resize(framebufferWidth, framebufferHeight);
		calculateViewProjection();
	}
	if (framebufferWidth == 0 || framebufferHeight == 0 || !has_context)
	{
		return;
	}

	std::chrono::high_resolution_clock::time_point now_time = std::chrono::high_resolution_clock::now();
	double elapsed = std::chrono::duration<double>(now_time - last_time).count();
	last_time = now_time;

	for (controller::controller_base::ptr controller : controllers) {
		if (!controller->has_model()) continue;
		controller->update(elapsed, wnd.get_user_input_object());
	}
    controllers.insert(controllers.end(), new_controllers.begin(), new_controllers.end());
    new_controllers.clear();
    // Remove views and controllers without model
    for (auto it = views.begin(); it != views.end();)
    {
        if (!it->get()->has_model())
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
        if (!it->get()->has_model())
        {
            it = controllers.erase(it);
        }
        else
        {
            ++it;
        }
    }
	if (cameraMode == stereo)
	{
		glEnable(GL_SCISSOR_TEST);

		// Render only to the left half.
		glViewport(0, 0, framebufferWidth / 2, framebufferHeight);
		glScissor(0, 0, framebufferWidth / 2, framebufferHeight);
		renderScene(leftProjection);

		// Render only to the right half.
		glViewport(framebufferWidth / 2, 0, framebufferWidth / 2, framebufferHeight);
		glScissor(framebufferWidth / 2, 0, framebufferWidth / 2, framebufferHeight);
		renderScene(rightProjection);

		// reset
		glDisable(GL_SCISSOR_TEST);
	}
	else	// mono
	{
		glViewport(0, 0, framebufferWidth, framebufferHeight);
		renderScene(leftProjection);
	}
}

void cgvkp::rendering::release_renderer::renderScene(glm::mat4x4 const& projection) const
{
	gbuffer.bindForGeometryPass();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    background.render();

	glm::vec3 ambientLight(0, 0, 0);
	for (auto const& light : pointLights)
	{
		ambientLight += light.ambientIntensity * light.color;
	}

	geometryPass.use();
	geometryPass.setAmbientLight(ambientLight);
	for (view::view_base::ptr v : views)
	{
        geometryPass.renderView(v, projection * viewMatrix);
	}
	glDepthMask(GL_FALSE);

    // Render lights
    renderLights(projection);

	// Copy final image into default framebuffer
	gbuffer.bindForFinalPass();
	glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, framebufferWidth, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void cgvkp::rendering::release_renderer::renderLights(glm::mat4x4 const& projection) const {
	// Render lights
	glEnable(GL_STENCIL_TEST);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_CLAMP);
	for (auto const& pointLight : pointLights)
	{
        renderPointLight(pointLight, projection);
	}
    PointLight p2;
    p2.color = glm::vec3(1, 1, 0.6f);
    p2.ambientIntensity = 0.25f;
    p2.diffuseIntensity = 0.2f;
    p2.constantAttenuation = 0;
    p2.linearAttenuation = 0;
    p2.exponentialAttenuation = 0.3f;
    for (auto const& view : views)
    {
        if (!view->is_valid() || !view->has_model() || !view->light_source()) continue;
        auto graphic_model = std::dynamic_pointer_cast<model::graphic_model_base>(view->get_model());
        if (graphic_model == nullptr) continue;
        p2.position = glm::vec3(graphic_model->model_matrix[3]);
        p2.calculateWorld();
        renderPointLight(p2, projection);
    }
	glDisable(GL_DEPTH_CLAMP);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
	glDisable(GL_STENCIL_TEST);
}

void cgvkp::rendering::release_renderer::renderPointLight(PointLight const& pointLight, glm::mat4x4 const& projection) const
{
    // Render shadow volumes.
    glDrawBuffer(GL_NONE);
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    shadowVolumePass.use();
    shadowVolumePass.setLightPosition(pointLight.position);
    shadowVolumePass.setViewProjection(projection * viewMatrix);
    for (view::view_base::ptr v : views)
    {
        shadowVolumePass.renderView(v);
    }

    glEnable(GL_CULL_FACE);

    // Render objects not in shadow.
    gbuffer.bindForLightPass();
    lightPass.use();
    lightPass.setScreenSize(framebufferWidth, framebufferHeight);
    lightPass.setEyePosition(viewMatrix[3].x, viewMatrix[3].y, viewMatrix[3].z);
    lightPass.setMaps();

    glDisable(GL_DEPTH_TEST);

    glStencilFunc(GL_EQUAL, 0x0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    lightPass.setWorldViewProjection(projection * viewMatrix * pointLight.world);
    lightPass.setLight(pointLight);
    lightingSphere.render();
}

void cgvkp::rendering::release_renderer::set_camera_mode(cgvkp::rendering::camera_mode mode)
{
	cameraMode = mode;
	calculateViewProjection();
}

void cgvkp::rendering::release_renderer::set_stereo_parameters(float eye_separation, float zzero_parallax)
{
	eyeSeparation = eye_separation;
	zZeroParallax = zzero_parallax;
	calculateViewProjection();
}

void cgvkp::rendering::release_renderer::lost_context()
{
    has_context = false;

	glBindVertexArray(0);
	glUseProgram(0);

    for (auto view : views)
    {
        view->deinit();
    }
	for (auto view : cached_views)
	{
		view->deinit();
	}

    background.deinit();
	lightingSphere.deinit();
	geometryPass.deinit();
	shadowVolumePass.deinit();
	lightPass.deinit();
	gbuffer.deinit();
}

bool cgvkp::rendering::release_renderer::restore_context(window const& wnd)
{
	wnd.get_size(framebufferWidth, framebufferHeight);
	wnd.make_current();

	if (!gbuffer.init(framebufferWidth, framebufferHeight))
	{
		return false;
	}

	if (!geometryPass.init())
	{
		return false;
	}
	if (!shadowVolumePass.init())
	{
		return false;
	}
	if (!lightPass.init())
	{
		return false;
	}

	if (!lightingSphere.init("meshes/lightingSphere.obj"))
	{
		return false;
	}
    if (!background.init())
    {
        return false;
    }

	for (auto view : cached_views)
	{
		view->init();
	}
	for (auto view : views)
	{
		view->init();
	}

	calculateViewProjection();
	has_context = true;
	return true;
}

void cgvkp::rendering::release_renderer::add_model(model::model_base::ptr model) {
	models.push_back(model);
}

void cgvkp::rendering::release_renderer::remove_model(model::model_base::ptr model) {
	auto it = std::find(models.begin(), models.end(), model);
	if (it != models.end())
	{
		models.erase(it);
	}
}

void cgvkp::rendering::release_renderer::add_view(view::view_base::ptr view) {
	views.push_back(view);
}

void cgvkp::rendering::release_renderer::add_controller(controller::controller_base::ptr controller) {
	new_controllers.push_back(controller);
}

float cgvkp::rendering::release_renderer::getDistance() {
	return distance;
}

float cgvkp::rendering::release_renderer::getAspect() {
	return aspect;
}
