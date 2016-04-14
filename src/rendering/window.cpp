#include "rendering/window.hpp"
#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

using namespace cgvkp;

namespace {

    void APIENTRY my_ogl_debug_func(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
        const char * type_str = "Default";
        const char *sever_str = "Default";
        switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB: type_str = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: type_str = "Deprecated Behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: type_str = "Undefined Behavior"; break;
            //case GL_DEBUG_TYPE_PORTABILITY_ARB: type_str = "Portability"; break;
            //case GL_DEBUG_TYPE_PERFORMANCE_ARB: type_str = "Performance"; break;
            //case GL_DEBUG_TYPE_OTHER_ARB: type_str = "Other"; break;
        default: return;
        }
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB: sever_str = "High"; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: sever_str = "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW_ARB: sever_str = "Low"; break;
            //default: return;
        }

        std::cerr << "ogl_dbg: " << type_str << ", " << sever_str << ", " << message << std::endl;
    }

}

rendering::window::window(unsigned int w, unsigned int h, const char* title) 
        : handle(nullptr) {
    ctor_impl(nullptr, w, h, title);
}

rendering::window::window(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title)
        : handle(nullptr) {
    ctor_impl(fullscreen, w, h, title);
}

rendering::window::~window() {
    dtor_impl();
}

void rendering::window::close() {
    ::glfwSetWindowShouldClose(this->handle, true);
}

bool rendering::window::is_alive() const {
    return (handle != nullptr)
        && (!::glfwWindowShouldClose(handle));
}

void rendering::window::do_events() {
    if (!is_alive()) return;
    ::glfwPollEvents();

    //// Keys
    //keys.update(handle);

    //// Cursor
    //glm::dvec2 position;
    //::glfwGetCursorPos(handle, &position.x, &position.y);
    //mouse_pos.x = static_cast<float>(position.x);
    //mouse_pos.y = static_cast<float>(position.y);

    //unsigned int w, h;
    //get_size(w, h);
    //if (h < 1) h = 1;
    //update_mouse_pos(static_cast<float>(w), static_cast<float>(h));

    //if (::glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
    //    if (!is_mouse_captured()) {
    //        capture_mouse();
    //    }
    //}
}

void rendering::window::make_current() const {
    if (!is_alive()) return;
    ::glfwMakeContextCurrent(handle);
}

void rendering::window::swap_buffers() const {
    if (!is_alive()) return;
    ::glfwSwapBuffers(handle);
}

bool rendering::window::get_size(unsigned int &out_width, unsigned int &out_height) const {
    int w, h;
    ::glfwGetFramebufferSize(handle, &w, &h);
    if (w < 0) w = 0;
    if (h < 0) h = 0;
    bool rv = (static_cast<unsigned int>(w) != out_width)
        || (static_cast<unsigned int>(h) != out_height);
    out_width = static_cast<unsigned int>(w);
    out_height = static_cast<unsigned int>(h);
    return rv;
}

void rendering::window::ctor_impl(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title) {
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // ogl 3.3 core
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (((w == -1) || (h == -1)) && (fullscreen != nullptr)) {
        const GLFWvidmode* mode = ::glfwGetVideoMode(fullscreen);
        w = mode->width;
        h = mode->height;
    }

    handle = ::glfwCreateWindow(w, h, title, fullscreen, NULL);

    if (!handle) {
        std::cerr << "glfwCreateWindow failed" << std::endl;
        return;
    }

    ::glfwMakeContextCurrent(handle);

    ::glewExperimental = GL_TRUE;
    GLenum err = ::glewInit();
    if (err != GLEW_OK) {
        dtor_impl();
        std::cerr << "glewInit failed" << std::endl;
        return;
    }
    if (!::glewIsSupported("GL_VERSION_3_3")) {
        dtor_impl();
        std::cerr << "glewIsSupported(GL_VERSION_3_3) failed" << std::endl;
        return;
    }

#ifdef GL_KHR_debug
    if (glewIsSupported("GL_KHR_debug")) {
        glDebugMessageCallback((GLDEBUGPROC)&my_ogl_debug_func, this);
        glEnable(GL_DEBUG_OUTPUT);
    }
#elif GL_ARB_debug_output
    if (glewIsSupported("GL_ARB_debug_output")) {
        glDebugMessageCallbackARB((GLDEBUGPROCARB)&my_ogl_debug_func, this);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    }
#endif

}

void rendering::window::dtor_impl() {
    if (handle != nullptr) {
        ::glfwDestroyWindow(handle);
        handle = nullptr;
    }
}
