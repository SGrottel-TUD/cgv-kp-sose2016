#pragma once

// forward declaration of internal GLFW types
struct GLFWwindow;

namespace cgvkp {
namespace rendering {

    class abstract_user_input {
    public:
        abstract_user_input();
        virtual ~abstract_user_input();

        virtual void mouse_button(GLFWwindow *window, int button, int action, int mods) = 0;
        virtual void mouse_wheel(GLFWwindow *window, double xoffset, double yoffset) = 0;
    };

}
}
