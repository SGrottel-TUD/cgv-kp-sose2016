#include "release_renderer.hpp"
#include "data/world.hpp"
#include "glm/gtx/transform.hpp"
#include "view/star_view.hpp"
#include "view/hand_view.hpp"
#include <iostream>

#include "controller/data_controller.hpp"

cgvkp::rendering::release_renderer::release_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
    models(), views(), cached_views(), controllers(), new_controllers(),
    vao(0), framebufferWidth(0), framebufferHeight(0), cameraMode(mono) {
}
cgvkp::rendering::release_renderer::~release_renderer(){}

bool cgvkp::rendering::release_renderer::init_impl(const window& wnd) {
	if (!restore_context(wnd))
	{
		return false;
	}

	float w = data.get_config().width();
	float h = data.get_config().height();
	view = glm::lookAt(glm::vec3(w / 2, 1.8f, 5.5f), glm::vec3(w / 2, 1.8f, - h / 2), glm::vec3(0, 1, 0));
	calculateProjection();

	exampleTechnique.init();

    // Create and add data controller
    controllers.push_back(std::make_shared<controller::data_controller>(this, data));

    // Create a cached hand view to avoid delay on first hand appearance.
    auto hand_view = std::make_shared<view::hand_view>();
    hand_view->init();
    cached_views.push_back(hand_view);

	return true;
}
void cgvkp::rendering::release_renderer::deinit_impl() {
	lost_context();
	exampleTechnique.deinit();
    for (auto view : views)
    {
        view->deinit();
    }
    for (auto view : cached_views)
    {
        view->deinit();
    }
}

void cgvkp::rendering::release_renderer::calculateProjection()
{
	float fovy = glm::quarter_pi<float>();
	float zNear = 0.01f;
	float zFar = 100;

	if (cameraMode == stereo)
	{
		float aspect = static_cast<float>(framebufferWidth / 2) / framebufferHeight;

		float tanHalfFovy = tan(fovy / 2);
		glm::vec3 translation = glm::vec3(-eyeSeparation * tanHalfFovy * zZeroParallax * aspect, 0.0f, 0.0f);

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
		float aspect = static_cast<float>(framebufferWidth) / framebufferHeight;
        if (aspect > glm::epsilon<float>())
		    leftProjection = glm::perspective(fovy, aspect, zNear, zFar);
	}
}

void cgvkp::rendering::release_renderer::render(const window& wnd) {
	if (framebufferWidth == 0 || framebufferHeight == 0 || !has_context)
	{
		return;
	}

	wnd.make_current();

	std::chrono::high_resolution_clock::time_point now_time = std::chrono::high_resolution_clock::now();
	double elapsed = std::chrono::duration<double>(now_time - last_time).count();
	last_time = now_time;

	for (controller::controller_base::ptr controller : controllers) {
		if (!controller->has_model()) continue;
		controller->update(elapsed, wnd.get_user_input_object());
	}
    controllers.insert(controllers.end(), new_controllers.begin(), new_controllers.end());
    new_controllers.clear();

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
		glViewport(0, 0, framebufferWidth, framebufferHeight);
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

	exampleTechnique.use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4x4 world(1);
	exampleTechnique.setWorldViewProjection(projection * view * world);

	for (view::view_base::ptr view : views) {
		if (!view->is_valid()) continue;
        auto graphic_model = std::dynamic_pointer_cast<model::graphic_model_base>(view->get_model());
        if (graphic_model != nullptr)
        {
            exampleTechnique.setModelMatrix(graphic_model->model_matrix);
        }
        view->render();
        // Rendering "Heartbeat"
        //std::cout << ".";
	}
}

void cgvkp::rendering::release_renderer::set_camera_mode(cgvkp::rendering::camera_mode mode)
{
	cameraMode = mode;
	calculateProjection();
}

void cgvkp::rendering::release_renderer::set_stereo_parameters(float eye_separation, float zzero_parallax)
{
	eyeSeparation = eye_separation;
	zZeroParallax = zzero_parallax;
	calculateProjection();
}

void cgvkp::rendering::release_renderer::set_framebuffer_size(int width, int height)
{
	framebufferWidth = width;
	framebufferHeight = height;
	calculateProjection();
}

void cgvkp::rendering::release_renderer::lost_context()
{
    std::cout << std::endl << "Lost context" << std::endl;
    has_context = false;
    for (auto view : views)
    {
        view->deinit();
    }
    for (auto view : cached_views)
    {
        view->deinit();
    }
	::glBindVertexArray(0);
	::glUseProgram(0);
	exampleTechnique.deinit();
}

bool cgvkp::rendering::release_renderer::restore_context(window const& wnd)
{
	wnd.get_size(framebufferWidth, framebufferHeight);

	wnd.make_current();

	::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	::glEnable(GL_DEPTH_TEST);
	::glEnable(GL_CULL_FACE);

	if (!exampleTechnique.init())
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
    std::cout << std::endl << "Got context" << std::endl;
    has_context = true;
	return true;
}
void cgvkp::rendering::release_renderer::add_model(model::model_base::ptr model) {
    models.push_back(model);
}
void cgvkp::rendering::release_renderer::add_view(view::view_base::ptr view) {
    views.push_back(view);
}
void cgvkp::rendering::release_renderer::add_controller(controller::controller_base::ptr controller) {
    new_controllers.push_back(controller);
}
