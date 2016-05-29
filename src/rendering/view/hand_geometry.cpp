#include "rendering/view/hand_geometry.hpp"
#include "util/resource_file.hpp"
#include "util/obj_parser.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace cgvkp {
namespace rendering {
namespace view {
    hand_geometry::hand_geometry() : shader(0u),
        model_loc(0u), projection_loc(0u), view_loc(0u),
        vao(0u), element_count(0u) {
    }
    hand_geometry::~hand_geometry() {
        ::glDeleteShader(shader);
        ::glBindVertexArray(0u);
        ::glDeleteVertexArrays(1, &vao);
        ::glDeleteBuffers(2, buffers);
    }
    bool hand_geometry::init() {
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

        // Set up geometry 
        std::vector<float> poss;
        std::vector<float> normals;

        util::obj_parser::parse("obj/hand1.obj", poss, normals);

        // Generate three buffers, vertex + normals
        ::glGenBuffers(2, buffers);
        // bind buffer for vertices and copy data into buffer
        ::glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        ::glBufferData(GL_ARRAY_BUFFER, poss.size() * sizeof(float), poss.data(), GL_STATIC_DRAW);

        ::glEnableVertexAttribArray(vertexLoc);
        ::glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        ::glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        ::glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

        ::glEnableVertexAttribArray(normalLoc);
        ::glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        ::glBindVertexArray(0);

        element_count = (unsigned int)poss.size();

        std::cout << "Model listing renderer initialized" << std::endl;
        return true;
    }
}
}
}