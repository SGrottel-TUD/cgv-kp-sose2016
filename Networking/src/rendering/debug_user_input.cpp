#include "rendering/debug_renderer.hpp"
#include "rendering/debug_user_input.hpp"

using namespace cgvkp;

rendering::debug_user_input::debug_user_input(data::input_layer& input_layer)
        : abstract_user_input(), vision::abstract_vision(input_layer), hands(), mbtns() {
    for (bool& b : mbtns) b = false;
}

rendering::debug_user_input::~debug_user_input() {
    // intentionally empty
}

bool rendering::debug_user_input::init_impl() {
    // intentionally empty
    return true;
}

void rendering::debug_user_input::update() {
    input_layer.buffer().assign(hands.begin(), hands.end());
    input_layer.sync_buffer();
}

void rendering::debug_user_input::deinit_impl() {
    // intentionally empty
}

void rendering::debug_user_input::mouse_button(GLFWwindow *window, int button, int action, int mods) {
    bool state = (action == GLFW_PRESS);
    if (mbtns[button] != state) {
        if (state) { // pressed down aka click
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                // create hand at location with height 50cm
                float x, y;
                mouse_pos_in_game_area(window, x, y);
                if (in_game_area(x, y)) {
                    int h = hand_at(x, y);
                    if (h < 0) {
                        hands.push_back({ x, y, 0.1f });
                    }
                }

            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                // delete hand at location
                float x, y;
                mouse_pos_in_game_area(window, x, y);
                int h = hand_at(x, y);
                if (h >= 0) {
                    std::vector<hand>::iterator hi = hands.begin();
                    hi += h;
                    hands.erase(hi);
                }
            }
        }
        mbtns[button] = state;
    }
}

void rendering::debug_user_input::mouse_wheel(GLFWwindow *window, double xoffset, double yoffset) {
    float x, y;
    mouse_pos_in_game_area(window, x, y);
    int h = hand_at(x, y);
    if (h >= 0) {
        hands[h].h += static_cast<float>(0.1 * yoffset);
        if (hands[h].h < 0.0) hands.erase(hands.begin() + h);
    }
}

void rendering::debug_user_input::mouse_pos_in_game_area(GLFWwindow* window, float& out_x, float& out_y) {
    double cx, cy;
    int wx, wy;
    ::glfwGetCursorPos(window, &cx, &cy);
    ::glfwGetWindowSize(window, &wx, &wy);
    // screen to opengl device
    cx = (cx / static_cast<double>(wx)) * 2.0 - 1.0;
    cy = -((cy / static_cast<double>(wy)) * 2.0 - 1.0);
    // ... to game area coordinates
    cx = ((cx / rendering::debug_renderer::presentation_scale) * 0.5 + 0.5) * input_layer.get_config().width();
    cy = ((cy / rendering::debug_renderer::presentation_scale) * 0.5 + 0.5) * input_layer.get_config().height();
    // and out
    out_x = static_cast<float>(cx);
    out_y = static_cast<float>(cy);
}

bool rendering::debug_user_input::in_game_area(float x, float y) {
    return (x >= 0.0f) && (x <= input_layer.get_config().width())
        && (y >= 0.0f) && (y <= input_layer.get_config().height());
}

int rendering::debug_user_input::hand_at(float x, float y) {
    float near_hand_dist = 10000.0f;
    std::vector<hand>::iterator near_hand = hands.end();
    std::vector<hand>::iterator hands_end = hands.end();
    double pos_eps_sq = input_layer.get_config().positional_epsilon();
    pos_eps_sq *= pos_eps_sq;
    for (std::vector<hand>::iterator hi = hands.begin(); hi != hands_end; ++hi) {
        float dx(hi->x - x);
        float dy(hi->y - y);
        float d(dx * dx + dy * dy);
        if (d > pos_eps_sq) continue; // too far apart
        if (d < near_hand_dist) {
            near_hand_dist = d;
            near_hand = hi;
        }
    }

    if (near_hand == hands.end()) return -1;

    return static_cast<int>(std::distance(hands.begin(), near_hand));
}
