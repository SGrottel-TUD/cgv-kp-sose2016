#include "release_renderer.hpp"
#include "data/world.hpp"
#include "glm/gtx/transform.hpp"
#include <iostream>

cgvkp::rendering::release_renderer::release_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
	models(), views(), controllers(), vao(0), framebufferWidth(0), framebufferHeight(0), cameraMode(mono) {
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

	return true;
}
void cgvkp::rendering::release_renderer::deinit_impl() {
	lost_context();
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

		leftProjection = glm::perspective(fovy, aspect, zNear, zFar);
	}
}

void cgvkp::rendering::release_renderer::render(const window& wnd) {
	if (framebufferWidth == 0 || framebufferHeight == 0)
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
	glBindVertexArray(vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	for (view::view_base::ptr view : views) {
		if (!view->is_valid()) continue;
		view->render();
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
	::glBindVertexArray(0);
	::glDeleteVertexArrays(1, &vao);
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

	::glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	// vertex buffer
	for (int i = 0; i < 2 * 3 * 3; ++i)
	{
		g_vertex_buffer_data[i] = 0;
	}
	float w = data.get_config().width();
	float h = data.get_config().height();

	g_vertex_buffer_data[3] = w;

	g_vertex_buffer_data[6] = w;
	g_vertex_buffer_data[8] = -h;

	g_vertex_buffer_data[12] = w;
	g_vertex_buffer_data[14] = -h;

	g_vertex_buffer_data[17] = -h;

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindVertexArray(0);

	return true;
}

/*
 * Example technique. Should be deleted in the near future.
*/

cgvkp::rendering::ExampleTechnique::ExampleTechnique()
	: Technique()
{
}

bool cgvkp::rendering::ExampleTechnique::init()
{
	if (!Technique::init())
	{
		return false;
	}
	if (!addShader(GL_VERTEX_SHADER, "res/shaders/example.vs"))
	{
		return false;
	}
	if (!addShader(GL_FRAGMENT_SHADER, "res/shaders/example.fs"))
	{
		return false;
	}
	if (!link())
	{
		return false;
	}

	worldViewProjectionLocation = getUniformLocation("worldViewProjection");

	if (worldViewProjectionLocation == -1)
	{
		return false;
	}

	return true;
}