#pragma once
#include <functional>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>


namespace cgvkp {
	class application_config;

namespace rendering {

    class abstract_user_input;

    class window
	{
    public:
		enum OnEvent
		{
			OnPress = 1,
			OnRepeat = 2,
			OnRelease = 4
		};

		window(application_config& config);
		~window();

		inline void close() const { glfwSetWindowShouldClose(handle, true); }
        inline bool is_alive() const { return handle != nullptr && !glfwWindowShouldClose(handle); }
        inline void do_events() const { if(is_alive()) glfwPollEvents(); }
		void reload();
		void setVsync();
        inline void make_current() const { if(handle) glfwMakeContextCurrent(handle); }
        inline void swap_buffers() const { if(is_alive()) glfwSwapBuffers(handle); }

        bool get_size(int &out_width, int &out_height) const;

		bool register_key_callback(int key, std::function<void()> const& callback, OnEvent ev = OnPress);
		inline void setMousePositionCallback(std::function<void(double, double)> const& callback) { setMousePosition = callback; }
		inline void setLeftMouseButtonCallback(std::function<void()> const& callback) { leftMouseButtonClick = callback; }
		inline void setInputCodePointCallback(std::function<bool(unsigned int)> const& callback) { inputCodePoint = callback; }

        inline std::shared_ptr<abstract_user_input> const& get_user_input_object(void) const { return user_input; }
        inline void set_user_input_object(std::shared_ptr<abstract_user_input> const& ui) { user_input = ui; }


    private:
		struct KeyEvents
		{
			std::function<void()> onPress;
			std::function<void()> onRepeat;
			std::function<void()> onRelease;
		};

		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void character_callback(GLFWwindow* window, unsigned int codepoint);
		static void mousePositionCallback(GLFWwindow* window, double x, double y);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void window_pos_callback(GLFWwindow* window, int xpos, int ypos);
			


		bool create_window();

        GLFWwindow* handle;
		std::string title;
		application_config& config;
		KeyEvents keys[GLFW_KEY_LAST + 1];
        std::shared_ptr<abstract_user_input> user_input;

		std::function<void(double, double)> setMousePosition;
		std::function<void()> leftMouseButtonClick;
		std::function<bool(unsigned int)> inputCodePoint;
    };


	inline window::OnEvent operator|(window::OnEvent a, window::OnEvent b)
	{
		return static_cast<window::OnEvent>(static_cast<int>(a) | static_cast<int>(b));
	}
}
}
