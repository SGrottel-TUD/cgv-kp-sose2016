#include "release_renderer.hpp"
#include "data/world.hpp"
#include "glm/gtx/transform.hpp"
#include <iostream>

cgvkp::rendering::release_renderer::release_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data),
	areaVao(0), sphereVao(0), framebufferWidth(0), framebufferHeight(0), cameraMode(mono) {
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

	// light source
	PointLight p1;
	p1.position = glm::vec3(0, 1.5f, 0);
	p1.color = glm::vec3(1, 1, 0.6f);
	p1.ambientIntensity = 0.05f;
	p1.diffuseIntensity = 0.8f;
	p1.constantAttenuation = 0;
	p1.linearAttenuation = 0;
	p1.exponentialAttenuation = 0.3f;
	p1.calculateWorld();
	pointLights.push_back(p1);

	PointLight p2;
	p2.position = glm::vec3(0, 1.5f, -3);
	p2.color = glm::vec3(1, 1, 0.6f);
	p2.ambientIntensity = 0.05f;
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

void cgvkp::rendering::release_renderer::render(const window& wnd)
{
	wnd.make_current();
	if (wnd.get_size(framebufferWidth, framebufferHeight))
	{
		gbuffer.resize(framebufferWidth, framebufferHeight);
		calculateProjection();
	}
	if (framebufferWidth == 0 || framebufferHeight == 0)
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
	glm::mat4x4 world = glm::translate(glm::vec3(data.get_config().width() / 2, 0.5f, -data.get_config().height() / 2)) * glm::rotate(static_cast<float>(glfwGetTime() / 2), glm::vec3(0, 1, 0));
	glm::mat4 worldBoden = glm::translate(glm::vec3(data.get_config().width() / 2, -0.005f, -data.get_config().height() / 2)) * glm::scale(glm::vec3(data.get_config().width(), 0.01f, data.get_config().height()));

	gbuffer.bindForGeometryPass();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 ambientLight(0, 0, 0);
	for (auto const& light : pointLights)
	{
		ambientLight += light.ambientIntensity * light.color;
	}

	geometryPass.use();
	geometryPass.setAmbientLight(ambientLight);
	{
		geometryPass.setWorld(world);
		geometryPass.setWorldViewProjection(projection * view * world);
		geometryPass.setMaterial();
		glBindVertexArray(areaVao);
		glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * 12, GL_UNSIGNED_INT, 0);

		geometryPass.setWorld(worldBoden);
		geometryPass.setWorldViewProjection(projection * view * worldBoden);
		geometryPass.setMaterial();
		glBindVertexArray(areaVao);
		glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * 12, GL_UNSIGNED_INT, 0);
	}
	glDepthMask(GL_FALSE);

	// Render lights
	glEnable(GL_STENCIL_TEST);
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_DEPTH_CLAMP);
	for (auto const& pointLight : pointLights)
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
		shadowVolumePass.setViewProjection(projection * view);
		{
			shadowVolumePass.setWorld(world);
			glBindVertexArray(areaVao);
			glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * 12, GL_UNSIGNED_INT, 0);

			shadowVolumePass.setWorld(worldBoden);
			glBindVertexArray(areaVao);
			glDrawElements(GL_TRIANGLES_ADJACENCY, 6 * 12, GL_UNSIGNED_INT, 0);
		}

		glEnable(GL_CULL_FACE);

		// Render objects not in shadow.
		gbuffer.bindForLightPass();
		lightPass.use();
		lightPass.setScreenSize(framebufferWidth, framebufferHeight);
		lightPass.setEyePosition(view[3].x, view[3].y, view[3].z);
		lightPass.setMaps();

		glDisable(GL_DEPTH_TEST);

		glStencilFunc(GL_EQUAL, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		lightPass.setWorldViewProjection(projection * view * pointLight.world);
		lightPass.setLight(pointLight);
		glBindVertexArray(sphereVao);
		glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, 0);
	}
	glDisable(GL_DEPTH_CLAMP);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
	glDisable(GL_STENCIL_TEST);

	// Copy final image into default framebuffer
	gbuffer.bindForFinalPass();
	glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, framebufferWidth, framebufferHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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

void cgvkp::rendering::release_renderer::lost_context()
{
	glBindVertexArray(0);
	glUseProgram(0);

	geometryPass.deinit();
	shadowVolumePass.deinit();
	lightPass.deinit();
	gbuffer.deinit();

	glDeleteVertexArrays(1, &areaVao);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);

	glDeleteVertexArrays(1, &sphereVao);
	glDeleteBuffers(1, &sphereVertexBuffer);
	glDeleteBuffers(1, &sphereIndexBuffer);
}

bool cgvkp::rendering::release_renderer::restore_context(window const& wnd)
{
	std::cout << "restore_context" << std::endl;
	wnd.get_size(framebufferWidth, framebufferHeight);
	wnd.make_current();
	calculateProjection();

	if (!gbuffer.init(framebufferWidth, framebufferHeight))
	{
		return false;
	}
	if (!geometryPass.init())
	{
		return false;
	}
	if (!lightPass.init())
	{
		return false;
	}
	if (!shadowVolumePass.init())
	{
		return false;
	}

	struct Vertex
	{
		Vertex() {}
		Vertex(glm::vec3 const& position, glm::vec3 const& normal, glm::vec2 const& textureCoord) { this->position = position; this->normal = normal; this->textureCoord = textureCoord; }
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoord;
	};


	float w = data.get_config().width();
	float h = data.get_config().height();
	Vertex g_vertex_buffer_data[6 * 4] = {
		// unten
		Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 0)),	// 0
		Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 0)),	// 1
		Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 1)),	// 2
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 1)),	// 3

		// oben
		Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0)),	// 4
		Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0)),	// 5
		Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 1)),	// 6
		Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 1)),	// 7

		// links
		Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 0)),	// 8
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 0)),	// 9
		Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 1)),	// 10
		Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 1)),	// 11

		// rechts
		Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0)),	// 12
		Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0)),	// 13
		Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1)),	// 14
		Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1)),	// 15

		// hinten
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 0)),	// 16
		Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 0)),	// 17
		Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 1)),	// 18
		Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 1)),	// 19

		// vorne
		Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 0)),	// 20
		Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 0)),	// 21
		Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 1)),	// 22
		Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 1)),	// 23
	};

	unsigned int indexBufferData[6 * 12] = {
		// unten
		0, 3, 2, 14, 1, 22,
		0, 10, 3, 16, 2, 1,

		// oben
		4, 20, 5, 12, 6, 7,
		4, 5, 6, 16, 7, 8,

		// links
		8, 11, 10, 18, 9, 2,
		8, 22, 11, 6, 10, 9,

		// rechts
		12, 0, 13, 16, 14, 15,
		12, 13, 14, 4, 15, 20,

		// hinten
		16, 19, 18, 12, 17, 0,
		16, 8, 19, 4, 18, 17,

		// vorne
		20, 2, 21, 14, 22, 23,
		20, 21, 22, 6, 23, 10,
	};

	glGenVertexArrays(1, &areaVao);
	glBindVertexArray(areaVao);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void const*>(12));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void const*>(24));

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData), indexBufferData, GL_STATIC_DRAW);
	glBindVertexArray(0);


	glm::vec3 sphereVertexBufferData[8] = {
		{ -1, -1, -1 },
		{ -1, -1, 1 },
		{ -1, 1, -1 },
		{ -1, 1, 1 },
		{ 1, -1, -1 },
		{ 1, -1, 1 },
		{ 1, 1, -1 },
		{ 1, 1, 1 },
	};
	unsigned int sphereIndexBufferData[6 * 2 * 3] = {
		// vorne
		1, 5, 7,
		1, 7, 3,

		// links
		0, 1, 3,
		0, 3, 2,

		// rechts
		5, 4, 6,
		5, 6, 7,

		// hinten
		0, 2, 6,
		0, 6, 4,

		// oben
		3, 7, 6,
		3, 6, 2,

		// unten
		5, 1, 0,
		5, 0, 4
	};

	glGenVertexArrays(1, &sphereVao);
	glBindVertexArray(sphereVao);

	glGenBuffers(1, &sphereVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertexBufferData), sphereVertexBufferData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &sphereIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereIndexBufferData), sphereIndexBufferData, GL_STATIC_DRAW);

	glBindVertexArray(0);

	return true;
}
