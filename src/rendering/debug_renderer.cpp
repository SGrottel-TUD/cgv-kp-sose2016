#include "debug_renderer.hpp"
#include "data/world.hpp"
#include "GL/glew.h"
#include <iostream>

using namespace cgvkp;

namespace {
    
    void printShaderInfoLog(GLuint obj) {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;
        ::glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
        if (infologLength > 0) {
            infoLog = (char *)malloc(infologLength);
            ::glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
            std::cout << infoLog << std::endl;
            free(infoLog);
        }
    }
    
    void printProgramInfoLog(GLuint obj) {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;
        ::glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
        if (infologLength > 0) {
            infoLog = (char *)malloc(infologLength);
            ::glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
            std::cout << infoLog << std::endl;
            free(infoLog);
        }
    }

}

const float rendering::debug_renderer::presentation_scale = 0.95f;

rendering::debug_renderer::debug_renderer(const ::cgvkp::data::world& data) 
    : abstract_renderer(data),
    vao(0), shader(0),
    win_w(0), win_h(0) {

}

rendering::debug_renderer::~debug_renderer() {

}

bool rendering::debug_renderer::init_impl(const window& wnd, std::string const& resourcesBasePath) {
    wnd.make_current();
	if (wnd.get_size(win_w, win_h)) {
		::glViewport(0, 0, win_w, win_h);
	}
    ::glClearColor(0.0f, 0.25f, 0.0f, 0.0f);
    ::glEnable(GL_DEPTH_TEST);
    ::glEnable(GL_CULL_FACE);

    //
    // Shaders
    //
    const char *vert_src = "#version 330 core\n\
uniform vec2 area;\n\
uniform vec2 p1;\n\
uniform vec2 p2;\n\
uniform float win_scale;\n\
\n\
void main() {\n\
    vec2 pos = (gl_VertexID == 0) ? p1 : p2;\n\
    gl_Position = vec4( ((pos / area) * 2.0 - 1.0) * win_scale, 0.0, 1.0);\n\
}";
    const char *frag_src = "#version 330 core\n\
uniform vec4 color;\n\
out vec4 o_color;\n\
void main() {\n\
    o_color = color;\n\
}";
    GLuint v, f;
    v = ::glCreateShader(GL_VERTEX_SHADER);
    f = ::glCreateShader(GL_FRAGMENT_SHADER);
    ::glShaderSource(v, 1, &vert_src, nullptr);
    ::glCompileShader(v);
    printShaderInfoLog(v);
    ::glShaderSource(f, 1, &frag_src, nullptr);
    ::glCompileShader(f);
    printShaderInfoLog(f);

    shader = ::glCreateProgram();
    ::glAttachShader(shader, v);
    ::glAttachShader(shader, f);
    //::glBindFragDataLocation(p, 0, "output_color");
    ::glLinkProgram(shader);
    printProgramInfoLog(shader);

    ::glUseProgram(shader);
    ::glUniform2f(::glGetUniformLocation(shader, "area"), static_cast<float>(data.get_config().width()), static_cast<float>(data.get_config().height()));
    ::glUniform1f(::glGetUniformLocation(shader, "win_scale"), presentation_scale);

    ::glGenVertexArrays(1, &vao);

    return true;
}

void rendering::debug_renderer::render(const window& wnd) {
	wnd.make_current();
	if (wnd.get_size(win_w, win_h))
	{
		::glViewport(0, 0, win_w, win_h);
	}

    if ((win_w == 0) || (win_h == 0)) return;

    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ::glDisable(GL_DEPTH_TEST);
    ::glDisable(GL_CULL_FACE);

    ::glUseProgram(shader);
    ::glBindVertexArray(vao);

    const float marker_size = 0.5f * data.get_config().positional_epsilon();
    const float marker_extra_size = 5.0f * marker_size;

    // game area bounding box
    set_color(1.0f, 1.0f, 1.0f);
    draw_line(0.0f, 0.0f, data.get_config().width(), 0.0f);
    draw_line(data.get_config().width(), 0.0f, data.get_config().width(), data.get_config().height());
    draw_line(data.get_config().width(), data.get_config().height(), 0.0f, data.get_config().height());
    draw_line(0.0f, data.get_config().height(), 0.0f, 0.0f);

    // markers for hands
    set_color(0.5f, 0.75f, 1.0f);
    for (const data::world::hand_ptr& hand : data.get_hands()) {
        draw_line(hand->x - marker_size, hand->y - marker_size, hand->x + marker_size, hand->y + marker_size);
        draw_line(hand->x - marker_size, hand->y + marker_size, hand->x + marker_size, hand->y - marker_size);

        draw_line(hand->x - 0.25f * marker_size, hand->y + marker_extra_size, hand->x + 0.25f * marker_size, hand->y + marker_extra_size);
        draw_line(hand->x, hand->y, hand->x, hand->y + marker_extra_size * hand->height);
    }

    // markers for stars
    set_color(1.0f, 0.9f, 0.0f);
    for (const data::world::star_ptr& star : data.get_stars()) {
        draw_line(star->x - marker_size, star->y - marker_size, star->x + marker_size, star->y + marker_size);
        draw_line(star->x - marker_size, star->y + marker_size, star->x + marker_size, star->y - marker_size);

        draw_line(star->x, star->y, star->x + marker_extra_size * star->dx, star->y + marker_extra_size * star->dy);
    }

}

void rendering::debug_renderer::deinit_impl() {
    ::glBindVertexArray(0);
    ::glDeleteVertexArrays(1, &vao);
    ::glUseProgram(0);
    ::glDeleteProgram(shader);
}

// some utility functions for simple und INEFFICIENT rendering

void rendering::debug_renderer::set_color(float r, float g, float b, float a) {
    ::glUniform4f(::glGetUniformLocation(shader, "color"), r, g, b, a);
}

void rendering::debug_renderer::set_p1(float x, float y) {
    ::glUniform2f(::glGetUniformLocation(shader, "p1"), x, y);
}

void rendering::debug_renderer::set_p2(float x, float y) {
    ::glUniform2f(::glGetUniformLocation(shader, "p2"), x, y);
}

void rendering::debug_renderer::draw_line() {
    ::glDrawArrays(GL_LINES, 0, 2);
}
