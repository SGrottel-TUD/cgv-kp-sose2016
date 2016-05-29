#include "rendering/view/star_geometry.hpp"
#include "util/resource_file.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace cgvkp {
namespace rendering {
namespace view {
    star_geometry::star_geometry() : shader(0u),
        model_loc(0u), projection_loc(0u), view_loc(0u),
        vao(0u), element_count(0u) {
    }
    star_geometry::~star_geometry() {
        ::glDeleteShader(shader);
        ::glBindVertexArray(0u);
        ::glDeleteVertexArrays(1, &vao);
        ::glDeleteBuffers(3, buffers);
    }
    bool star_geometry::init() {
        //
        // Shaders
        //
        std::string path = cgvkp::util::resource_file::find_resource_file("shaders/star_vs");
		std::string vert_src;
		cgvkp::util::resource_file::read_file_as_text(path.c_str(), vert_src);
        path = cgvkp::util::resource_file::find_resource_file("shaders/star_fs");
		std::string frag_src;
		cgvkp::util::resource_file::read_file_as_text(path.c_str(), frag_src);
        const char *c_str;
        GLuint v, f;
        v = ::glCreateShader(GL_VERTEX_SHADER);
        f = ::glCreateShader(GL_FRAGMENT_SHADER);
        c_str = vert_src.data();
        ::glShaderSource(v, 1, &c_str, nullptr);
        ::glCompileShader(v);
        //printShaderInfoLog(v);
        c_str = frag_src.data();
        ::glShaderSource(f, 1, &c_str, nullptr);
        ::glCompileShader(f);
        //printShaderInfoLog(f);

        shader = ::glCreateProgram();
        ::glAttachShader(shader, v);
        ::glAttachShader(shader, f);
        //::glBindFragDataLocation(p, 0, "output_color");
        ::glLinkProgram(shader);
        //printProgramInfoLog(shader);

        ::glUseProgram(shader);

        model_loc = ::glGetUniformLocation(shader, "model");
        projection_loc = ::glGetUniformLocation(shader, "projection");
        view_loc = ::glGetUniformLocation(shader, "view");

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

        element_count = (unsigned int)indices.size();

        std::cout << "Model listing renderer initialized" << std::endl;
        return true;
    }
}
}
}