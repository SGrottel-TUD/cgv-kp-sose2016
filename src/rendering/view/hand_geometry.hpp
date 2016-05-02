#pragma once
#include "rendering/view/view_geometry_base.hpp"
#include "GL/glew.h"
#include <glm/glm.hpp>

namespace cgvkp {
namespace rendering {
namespace view {
    class hand_geometry {
    public:
        ~hand_geometry();
        hand_geometry();
        bool init();
        GLuint shader;
        GLuint model_loc, projection_loc, view_loc;
        GLuint vao;
        GLuint buffers[2];
        GLuint element_count;
    };
}
}
}
