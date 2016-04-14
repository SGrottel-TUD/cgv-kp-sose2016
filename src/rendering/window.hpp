#pragma once

// forward declaration of internal GLFW types
struct GLFWmonitor;
struct GLFWwindow;

namespace cgvkp {
namespace rendering {

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

    private:
        void ctor_impl(GLFWmonitor* fullscreen, unsigned int w, unsigned int h, const char* title);
        void dtor_impl();

        GLFWwindow *handle;
    };

}
}
