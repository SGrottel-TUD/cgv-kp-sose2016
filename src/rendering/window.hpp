#pragma once
#include <memory>
#include <string>
#include <functional>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace cgvkp {
namespace rendering {

    class abstract_user_input;

    class window {
    public:
		enum on_event {
			OnPress = 1,
			OnRepeat = 2,
			OnRelease = 4
		};

        window(unsigned int w, unsigned int h, const char* title = "CGV KP SoSe2016");
        // Use a width of '-1' and a monitor, to keep that monitors video mode
        window(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title = "CGV KP SoSe2016");
        ~window();

        void close();
        bool is_alive() const;
        void do_events();
		void toggle_fullscreen();
        void make_current() const;
        void swap_buffers() const;

        bool get_size(int &out_width, int &out_height) const;

		bool register_key_callback(int key, std::function<void()> callback, on_event ev = OnPress);
		inline void register_framebuffer_size_callback(std::function<void(int, int)> callback) { framebuffer_size_cb = callback; }

        inline const std::shared_ptr<abstract_user_input> get_user_input_object(void) const {
            return user_input;
        }
        inline void set_user_input_object(std::shared_ptr<abstract_user_input> ui) {
            user_input = ui;
        }


    private:
		struct key_events {
			std::function<void()> onPress;
			std::function<void()> onRepeat;
			std::function<void()> onRelease;
		};

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        void ctor_impl(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title);
        void dtor_impl();
		bool create_window(int width, int height, char const* title, GLFWmonitor* fullscreen);

        GLFWwindow* handle;
		std::string title;
		bool fullscreen;
		int windowed_x;
		int windowed_y;
		int windowed_width;
		int windowed_height;
		key_events keys[GLFW_KEY_LAST + 1];
        std::shared_ptr<abstract_user_input> user_input;
		std::function<void(int, int)> framebuffer_size_cb;
    };


	inline window::on_event operator|(window::on_event a, window::on_event b)
	{
		return static_cast<window::on_event>(static_cast<int>(a) | static_cast<int>(b));
	}
}
}
