#pragma once
#include <memory>
#include <string>

// forward declaration of internal GLFW types
struct GLFWmonitor;
struct GLFWwindow;

#define GetKeyOnce(w, k) (glfwGetKey(w, k) ? (keys[k] ? false : (keys[k] = true)) : (keys[k] = false))

namespace cgvkp {
namespace rendering {

    class abstract_user_input;

    class window {
    public:
        window(unsigned int w, unsigned int h, const char* title = "CGV KP SoSe2016");
        // Use a width of '-1' and a monitor, to keep that monitors video mode
        window(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title = "CGV KP SoSe2016");
        ~window();

        void close();
        bool is_alive() const;
        bool do_events();
		void toggle_fullscreen();
        void make_current() const;
        void swap_buffers() const;

        bool get_size(unsigned int &out_width, unsigned int &out_height) const;

        inline const std::shared_ptr<abstract_user_input> get_user_input_object(void) const {
            return user_input;
        }
        inline void set_user_input_object(std::shared_ptr<abstract_user_input> ui) {
            user_input = ui;
        }


    private:

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        void ctor_impl(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title);
        void dtor_impl();
		bool create_window(int width, int height, char const* title, GLFWmonitor* fullscreen);

        GLFWwindow *handle;
		std::string title;
		bool fullscreen;
		int windowed_x;
		int windowed_y;
		int windowed_width;
		int windowed_height;
		bool keys[256];
        std::shared_ptr<abstract_user_input> user_input;
    };

}
}
