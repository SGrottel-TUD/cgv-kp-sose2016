#include "rendering/view/hand_geometry.hpp"
#include "util/resource_file.hpp"
#include "util/obj_parser.hpp"
#include "rendering/technique.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace cgvkp {
namespace rendering {
namespace view {
    hand_geometry::hand_geometry() :
        vao(0u), element_count(0u) {
    }
    hand_geometry::~hand_geometry() {
        ::glBindVertexArray(0u);
        ::glDeleteVertexArrays(1, &vao);
        ::glDeleteBuffers(2, buffers);
    }
    bool hand_geometry::init() {

        ::glGenVertexArrays(1, &vao);
        ::glBindVertexArray(vao);

        // Set up geometry 
        std::vector<float> poss;
        std::vector<float> normals;

        util::obj_parser::parse("obj/hand1.obj", poss, normals);

        // Generate three buffers, vertex + normals
        ::glGenBuffers(2, buffers);
        // bind buffer for vertices and copy data into buffer
        ::glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        ::glBufferData(GL_ARRAY_BUFFER, poss.size() * sizeof(float), poss.data(), GL_STATIC_DRAW);

        ::glEnableVertexAttribArray(IN_LOC_POSITION);
        ::glVertexAttribPointer(IN_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        ::glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        ::glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

        ::glEnableVertexAttribArray(IN_LOC_NORMAL);
        ::glVertexAttribPointer(IN_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        ::glBindVertexArray(0);

        element_count = (unsigned int)poss.size();

        std::cout << "Model listing renderer initialized" << std::endl;
        return true;
    }
}
}
}