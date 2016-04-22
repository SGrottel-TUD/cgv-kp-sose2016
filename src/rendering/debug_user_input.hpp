#pragma once
#include "rendering/abstract_user_input.hpp"
#include "data/input_layer.hpp"
#include "GLFW/glfw3.h"
#include <vector>
#include <array>

namespace cgvkp {
namespace rendering {

    class debug_user_input : public abstract_user_input {
    public:
        typedef data::input_layer::hand hand;

        debug_user_input(data::input_layer& input_layer);
        virtual ~debug_user_input();

        bool init();
        void update();
        void deinit();

        virtual void mouse_button(GLFWwindow *window, int button, int action, int mods);
        virtual void mouse_wheel(GLFWwindow *window, double xoffset, double yoffset);

    private:
        data::input_layer& input_layer;

        void mouse_pos_in_game_area(GLFWwindow* window, float& out_x, float& out_y);
        bool in_game_area(float x, float y);
        int hand_at(float x, float y);

        std::vector<hand> hands;
        std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mbtns;
    };

}
}

