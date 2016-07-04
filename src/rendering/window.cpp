#include "rendering/window.hpp"
#include <iostream>
#include "rendering/abstract_user_input.hpp"

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
        : handle(nullptr), user_input(), fullscreen(false), windowed_width(w), windowed_height(h), windowed_x(0), windowed_y(0) {
    ctor_impl(nullptr, w, h, title);
}

rendering::window::window(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title)
        : handle(nullptr), user_input() {
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
	if (handle != nullptr)
	{
		::glfwMakeContextCurrent(handle);
	}
}

void rendering::window::swap_buffers() const {
    if (!is_alive()) return;
    ::glfwSwapBuffers(handle);
}

bool rendering::window::register_key_callback(int key, std::function<void()> callback, on_event ev /* = OnDown */)
{
	if (key > GLFW_KEY_LAST)
	{
		return false;
	}

	key_events& k = keys[key];

	if (ev & OnPress)
	{
		k.onPress = callback;
	}
	if (ev & OnRepeat)
	{
		k.onRepeat = callback;
	}
	if (ev & OnRelease)
	{
		k.onRelease = callback;
	}

	return true;
}

bool rendering::window::get_size(int &out_width, int &out_height) const {
    int w, h;
    ::glfwGetFramebufferSize(handle, &w, &h);
    if (w < 0) w = 0;
    if (h < 0) h = 0;
    bool rv = (w != out_width) || (h != out_height);
    out_width = w;
    out_height = h;
    return rv;
}

void rendering::window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UNKNOWN)
	{
		return;
	}
	rendering::window* w = static_cast<rendering::window*>(glfwGetWindowUserPointer(window));
	if (key <= GLFW_KEY_LAST)
	{
		key_events const& k = w->keys[key];
		if (action == GLFW_PRESS && k.onPress)
		{
			k.onPress();
		}
		else if (action == GLFW_REPEAT && k.onRepeat)
		{
			k.onRepeat();
		}
		else if (action == GLFW_RELEASE && k.onRelease)
		{
			k.onRelease();
		}
	}
}

void rendering::window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    rendering::window *that = static_cast<rendering::window*>(::glfwGetWindowUserPointer(window));
    if (that->user_input) that->user_input->mouse_button(window, button, action, mods);
}

void rendering::window::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    rendering::window *that = static_cast<rendering::window*>(::glfwGetWindowUserPointer(window));
    if (that->user_input) that->user_input->mouse_wheel(window, xoffset, yoffset);
}

void rendering::window::ctor_impl(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title) {
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // ogl 3.3 core
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	this->title = title;
	create_window(w, h, title, fullscreen);

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
    user_input.reset();
    if (handle != nullptr) {
        ::glfwDestroyWindow(handle);
        handle = nullptr;
    }
}

bool rendering::window::create_window(int width, int height, char const* title, GLFWmonitor* fullscreen) {
	if (fullscreen != nullptr) {
		const GLFWvidmode* mode = ::glfwGetVideoMode(fullscreen);

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		if (width == -1 || height == -1) {
			width = mode->width;
			height = mode->height;
		}
	}

	handle = ::glfwCreateWindow(width, height, title, fullscreen, nullptr);

	if (!handle) {
		std::cerr << "glfwCreateWindow failed" << std::endl;
		return false;
	}

	this->fullscreen = fullscreen != nullptr;

	::glfwSetWindowUserPointer(handle, this);

	::glfwSetMouseButtonCallback(handle, window::mouse_button_callback);
	::glfwSetScrollCallback(handle, window::mouse_scroll_callback);
	::glfwSetKeyCallback(handle, window::key_callback);

	::glfwMakeContextCurrent(handle);
	//glfwSwapInterval(1);

	return true;
}

void rendering::window::toggle_fullscreen() {
	if (fullscreen) {
		glfwDestroyWindow(handle);

		if(create_window(windowed_width, windowed_height, title.c_str(), nullptr)) {
			glfwSetWindowPos(handle, windowed_x, windowed_y);
		}
	}
	else {
		glfwGetWindowPos(handle, &windowed_x, &windowed_y);
		glfwGetWindowSize(handle, &windowed_width, &windowed_height);

		glfwDestroyWindow(handle);

		create_window(-1, -1, title.c_str(), glfwGetPrimaryMonitor());
	}
}
