#include "model_listing_renderer.hpp"
#include "data/world.hpp"
#include "rendering/window.hpp"
#include "GL/glew.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

cgvkp::rendering::model_listing_renderer::model_listing_renderer(const ::cgvkp::data::world& data)
    : cgvkp::rendering::abstract_renderer(data) {
}
cgvkp::rendering::model_listing_renderer::~model_listing_renderer() {}

bool cgvkp::rendering::model_listing_renderer::init_impl(const window& wnd) {
    wnd.make_current();
    ::glClearColor(0.0f, 0.25f, 0.0f, 0.0f);
    ::glEnable(GL_DEPTH_TEST);
    ::glEnable(GL_CULL_FACE);

    //
    // Shaders
    //
    const char *vert_src = "#version 330 core\n\
in vec3 vert_position;\n\
in vec3 vert_normal;\n\
out vec3 frag_position;\n\
out vec3 frag_normal;\n\
\n\
void main() {\n\
    frag_position = abs(vert_position);\n\
    frag_normal = abs(vert_normal);\n\
    gl_Position = vec4(vert_position, 1.0);\n\
}";
    const char *frag_src = "#version 330 core\n\
in vec3 frag_position;\n\
in vec3 frag_normal;\n\
out vec4 o_color;\n\
\n\
void main() {\n\
    o_color = vec4(frag_position + frag_normal, 1.0);\n\
}";
    GLuint v, f;
    v = ::glCreateShader(GL_VERTEX_SHADER);
    f = ::glCreateShader(GL_FRAGMENT_SHADER);
    ::glShaderSource(v, 1, &vert_src, nullptr);
    ::glCompileShader(v);
    //printShaderInfoLog(v);
    ::glShaderSource(f, 1, &frag_src, nullptr);
    ::glCompileShader(f);
    //printShaderInfoLog(f);

    shader = ::glCreateProgram();
    ::glAttachShader(shader, v);
    ::glAttachShader(shader, f);
    //::glBindFragDataLocation(p, 0, "output_color");
    ::glLinkProgram(shader);
    //printProgramInfoLog(shader);

    ::glUseProgram(shader);

    ::glGenVertexArrays(1, &vao);
    ::glBindVertexArray(vao);

    GLuint vertexLoc = ::glGetAttribLocation(shader, "vert_position");
    GLuint normalLoc = ::glGetAttribLocation(shader, "vert_normal");

    // Calculate star positions
    std::vector<float> poss, normals;
    std::vector<uint32_t> indices;

    poss.reserve(3 * 3 * 20); // 3 points / face
    normals.reserve(3 * 3 * 20);
    indices.reserve(3 * 20); // 20 faces
    for (int i = 0; i < indices.capacity(); ++i)
        indices.push_back(i);

    float star_depth = 0.2f;
    // "Top" center vertex
    glm::vec3 top = glm::vec3(0.0f, 0.0f, star_depth);
    // "Bottom" center vertex
    glm::vec3 bot = glm::vec3(0.0f, 0.0f, -star_depth);

    // Generate data, iterate through peaks
    float angle_step = glm::pi<float>() / 5.0f;
    for (int i = 0; i < 5u; ++i) {
        float theta = 2 * i * angle_step;
        glm::vec3 peak = glm::vec3(glm::cos(theta), glm::sin(theta), 0.0f);
        glm::vec3 left = 0.5f * glm::vec3(glm::cos(theta + angle_step), glm::sin(theta + angle_step), 0.0f);
        glm::vec3 right = 0.5f * glm::vec3(glm::cos(theta - angle_step), glm::sin(theta - angle_step), 0.0f);

        glm::vec3 vl, vr, vt, vb, n;
        vl = left - peak;
        vr = right - peak;
        vt = top - peak;
        vb = bot - peak;

        n = glm::normalize(glm::cross(vl, vt));
        poss.push_back(peak.x); poss.push_back(peak.y); poss.push_back(peak.z);
        poss.push_back(left.x); poss.push_back(left.y); poss.push_back(left.z);
        poss.push_back(top.x); poss.push_back(top.y); poss.push_back(top.z);
        for (int j = 0; j < 3; ++j) {
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
        }

        n = glm::normalize(glm::cross(vb, vl));
        poss.push_back(left.x); poss.push_back(left.y); poss.push_back(left.z);
        poss.push_back(peak.x); poss.push_back(peak.y); poss.push_back(peak.z);
        poss.push_back(bot.x); poss.push_back(bot.y); poss.push_back(bot.z);
        for (int j = 0; j < 3; ++j) {
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
        }

        n = glm::normalize(glm::cross(vt, vr));
        poss.push_back(peak.x); poss.push_back(peak.y); poss.push_back(peak.z);
        poss.push_back(top.x); poss.push_back(top.y); poss.push_back(top.z);
        poss.push_back(right.x); poss.push_back(right.y); poss.push_back(right.z);
        for (int j = 0; j < 3; ++j) {
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
        }

        n = glm::normalize(glm::cross(vr, vb));
        poss.push_back(peak.x); poss.push_back(peak.y); poss.push_back(peak.z);
        poss.push_back(right.x); poss.push_back(right.y); poss.push_back(right.z);
        poss.push_back(bot.x); poss.push_back(bot.y); poss.push_back(bot.z);
        for (int j = 0; j < 3; ++j) {
            normals.push_back(n.x); normals.push_back(n.y); normals.push_back(n.z);
        }
    }

    // Generate three buffers, indices + vertex + normals
    ::glGenBuffers(3, buffers);
    // bind buffer for vertices and copy data into buffer
    ::glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    ::glBufferData(GL_ARRAY_BUFFER, poss.size() * sizeof(float), poss.data(), GL_STATIC_DRAW);

    ::glEnableVertexAttribArray(vertexLoc);
    ::glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    ::glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    ::glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

    ::glEnableVertexAttribArray(normalLoc);
    ::glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    ::glBindVertexArray(0);

    element_count = indices.size();

    std::cout << "Model listing renderer initialized" << std::endl;
    return true;
}
void cgvkp::rendering::model_listing_renderer::deinit_impl() {
	::glBindVertexArray(0);
	::glDeleteBuffers(2, buffers);
	::glDeleteVertexArrays(1, &vao);
}

void cgvkp::rendering::model_listing_renderer::render(const window& wnd) {
    /*
    if (wnd.get_size(win_w, win_h)) {
        ::glViewport(0, 0, win_w, win_h);
    }
    if ((win_w == 0) || (win_h == 0)) return;
    */
    ::glUseProgram(shader);

    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ::glDisable(GL_DEPTH_TEST);
    ::glDisable(GL_CULL_FACE);

    ::glBindVertexArray(vao);
    ::glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, nullptr);

}