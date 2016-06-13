#include "model_listing_renderer.hpp"
#include "data/world.hpp"
#include "rendering/window.hpp"
#include "GL/glew.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

cgvkp::rendering::model_listing_renderer::model_listing_renderer(const ::cgvkp::data::world& data)
	: cgvkp::rendering::abstract_renderer(data),
    star_model(std::make_shared<model::star_model>()),
    star_view(std::make_shared<view::star_view>()),
    hand_model(std::make_shared<model::hand_model>()),
    hand_view(std::make_shared<view::hand_view>()) {
}
cgvkp::rendering::model_listing_renderer::~model_listing_renderer() {}

bool cgvkp::rendering::model_listing_renderer::init_impl(const window& wnd) {
    wnd.make_current();

	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	::glDisable(GL_CULL_FACE);
	::glEnable(GL_DEPTH_TEST);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	::glDepthMask(GL_TRUE);

    technique1.init();

    star_model->model_matrix =
        glm::translate(glm::vec3(-0.5f, 0.0f, 0.0f)) *
        glm::scale(glm::vec3(2.5f)) *
        star_model->model_matrix;
    star_view->set_model(star_model);
    star_view->init();

    hand_model->model_matrix =
        glm::translate(glm::vec3(0.5f, 0.0f, 0.0f)) *
        glm::scale(glm::vec3(2.0f)) *
        hand_model->model_matrix;
    hand_view->set_model(hand_model);
    hand_view->init();

    std::cout << "Model listing renderer initialized" << std::endl;
    return true;
}
void cgvkp::rendering::model_listing_renderer::deinit_impl() {
    star_view->deinit();
    star_view.reset();
    star_model.reset();

    hand_view->deinit();
    hand_view.reset();
    hand_model.reset();

    technique1.deinit();
}

void cgvkp::rendering::model_listing_renderer::render(const window& wnd) {
	wnd.make_current();
    /*
    if (wnd.get_size(win_w, win_h)) {
        ::glViewport(0, 0, win_w, win_h);
    }
    if ((win_w == 0) || (win_h == 0)) return;
    */
	// Update model_matrix
	std::chrono::high_resolution_clock::time_point now_time = std::chrono::high_resolution_clock::now();
	float elapsed = std::chrono::duration<float>(now_time - last_time).count();
	last_time = now_time;

	star_model->model_matrix *= glm::rotate(0.5f * elapsed,
		glm::vec3(1.0f, 0.0f, 0.0f));
	hand_model->model_matrix *= glm::rotate(-0.5f * elapsed,
		glm::vec3(0.0f, 0.0f, 1.0f));

	::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    technique1.use();
    technique1.setWorldViewProjection(glm::mat4(1.0f));
    technique1.setModelMatrix(star_model->model_matrix);
    star_view->render();
    technique1.setModelMatrix(hand_model->model_matrix);
    hand_view->render();
}