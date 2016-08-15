#include <iostream>
#include <GL/glew.h>
#include "abstract_user_input.hpp"
#include "window.hpp"
#include "application_config.hpp"

using namespace cgvkp;

namespace
{
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

rendering::window::window(application_config& _config)
	: config(_config)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // ogl 3.3 core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	title = "CGV KP SoSe2016";
	if (!create_window())
	{
		return;
	}

	::glewExperimental = GL_TRUE;
	GLenum err = ::glewInit();
	if (err != GLEW_OK) {
		glfwDestroyWindow(handle);
		handle = nullptr;
		std::cerr << "glewInit failed" << std::endl;
		return;
	}
	if (!::glewIsSupported("GL_VERSION_3_3")) {
		glfwDestroyWindow(handle);
		handle = nullptr;
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

rendering::window::~window()
{
	user_input.reset();
	if (handle != nullptr)
	{
		glfwDestroyWindow(handle);
	}
}

bool rendering::window::register_key_callback(int key, std::function<void()> const& callback, OnEvent ev /* = OnDown */)
{
	if (key > GLFW_KEY_LAST)
	{
		return false;
	}

	auto& k = keys[key];

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

bool rendering::window::create_window()
{
	if (config.fullscreen) 
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = ::glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		
		if (config.fullscreenHeight == 0 || config.fullscreenWidth == 0)
		{
			config.fullscreenHeight = mode->height;
			config.fullscreenWidth = mode->width;
		}

		handle = ::glfwCreateWindow(config.fullscreenWidth, config.fullscreenHeight, title.c_str(), monitor, nullptr);
	}
	else
	{
		handle = ::glfwCreateWindow(config.windowWidth, config.windowHeight, title.c_str(), nullptr, nullptr);
	}


	if (!handle) {
		std::cerr << "glfwCreateWindow failed" << std::endl;
		return false;
	}

	glfwSetWindowUserPointer(handle, this);

	glfwSetCursorPosCallback(handle, mousePositionCallback);
	glfwSetMouseButtonCallback(handle, mouse_button_callback);
	glfwSetScrollCallback(handle, mouse_scroll_callback);
	glfwSetKeyCallback(handle, key_callback);
	glfwSetCharCallback(handle, character_callback);
	glfwSetWindowPosCallback(handle, window_pos_callback);

	glfwMakeContextCurrent(handle);
	
	if (config.vSync)
	{
		glfwSwapInterval(1);
	}
	else if (glfwExtensionSupported("WGL_EXT_swap_control_tear") || glfwExtensionSupported("GLX_EXT_swap_control_tear"))
	{
		glfwSwapInterval(-1);
	}
	else
	{
		glfwSwapInterval(0);
	}

	if (!config.fullscreen)
	{
		glfwSetWindowPos(handle, config.windowPosx, config.windowPosy);
	}

	return true;
}

void rendering::window::toggle_fullscreen()
{
	glfwDestroyWindow(handle);
	config.fullscreen = !config.fullscreen;
	create_window();
}

void rendering::window::mousePositionCallback(GLFWwindow* window, double x, double y)
{
	rendering::window* w = static_cast<rendering::window*>(glfwGetWindowUserPointer(window));
	if (w->setMousePosition)
	{
		w->setMousePosition(x, y);
	}
}

void rendering::window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	rendering::window *win = static_cast<rendering::window*>(::glfwGetWindowUserPointer(window));
	if (win->leftMouseButtonClick && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		win->leftMouseButtonClick();
	}
	else if (win->user_input)
	{
		win->user_input->mouse_button(window, button, action, mods);
	}
}

void rendering::window::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	rendering::window *that = static_cast<rendering::window*>(::glfwGetWindowUserPointer(window));
	if (that->user_input) that->user_input->mouse_wheel(window, xoffset, yoffset);
}

void rendering::window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	rendering::window* w = static_cast<rendering::window*>(glfwGetWindowUserPointer(window));

	if (w->inputCodePoint && key)
	{
		if (w->inputCodePoint(0))
		{
			if (key > GLFW_KEY_ESCAPE && (action == GLFW_PRESS || action == GLFW_REPEAT))
			{
				switch (key)
				{
				case GLFW_KEY_BACKSPACE:
					w->inputCodePoint(0x08);
					break;
				}
			}
			return;
		}
	}

	if (key <= GLFW_KEY_LAST)
	{
		auto const& k = w->keys[key];
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

void rendering::window::character_callback(GLFWwindow* window, unsigned int codepoint)
{
	rendering::window* w = static_cast<rendering::window*>(glfwGetWindowUserPointer(window));
	if (w->inputCodePoint)
	{
		w->inputCodePoint(codepoint);
	}
} 

void rendering::window::window_pos_callback(GLFWwindow* window, int xpos, int ypos)
{
	rendering::window* w = static_cast<rendering::window*>(glfwGetWindowUserPointer(window));
	w->config.windowPosx = xpos;
	w->config.windowPosy = ypos;
}