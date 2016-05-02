#pragma once
#include "rendering/view/view_geometry_base.hpp"
#include "GL/glew.h"
#include <glm/glm.hpp>

namespace cgvkp {
namespace rendering {
namespace view {
    class star_geometry {
    public:
        ~star_geometry();
        star_geometry();
        bool init();
        GLuint shader;
        GLuint model_loc, projection_loc, view_loc;
        GLuint vao;
        GLuint buffers[3];
        GLuint element_count;
    };
}
}
}