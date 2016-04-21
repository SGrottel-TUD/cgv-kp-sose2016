#include "debug_renderer.hpp"
#include "GL/glew.h"

using namespace cgvkp;

rendering::debug_renderer::debug_renderer(const data::world& data) : data(data) {

}

rendering::debug_renderer::~debug_renderer() {

}

bool rendering::debug_renderer::init(const window& wnd) {
    wnd.make_current();
    ::glClearColor(0.0f, 0.75f, 0.0f, 0.0f);
    ::glEnable(GL_DEPTH_TEST);
    ::glEnable(GL_CULL_FACE);
    return true;
}

void rendering::debug_renderer::render(const window& wnd) {

    //main_window->update_size(scene.camera());
    //main_window->update_cursor(scene.cursor());
    //manage_views_and_controllers();
    //update_models();
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //render_views();

}

void rendering::debug_renderer::deinit() {

}
