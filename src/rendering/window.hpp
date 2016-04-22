#pragma once
#include <memory>

// forward declaration of internal GLFW types
struct GLFWmonitor;
struct GLFWwindow;

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
        void do_events();
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

        void ctor_impl(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title);
        void dtor_impl();

        GLFWwindow *handle;
        std::shared_ptr<abstract_user_input> user_input;
    };

}
}
