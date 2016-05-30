#include "rendering/view/hand_view.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace cgvkp {
namespace rendering {
namespace view {
    hand_view::hand_view() : view_geometry_base<hand_geometry>() {
    }
    hand_view::~hand_view() {
    }
    std::shared_ptr<model::hand_model> hand_view::get_model() const {
        return std::dynamic_pointer_cast<model::hand_model>(model.lock());
    }
    void hand_view::render() {
        auto m = get_model();
        ::glDisable(GL_CULL_FACE);
        ::glEnable(GL_DEPTH_TEST);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glDepthMask(GL_TRUE);;

        ::glBindVertexArray(geo->vao);

        glm::mat4 view_matrix = glm::mat4(1.0f);
        glm::mat4 projection_matrix = glm::mat4(1.0f);

        ::glDrawArrays(GL_TRIANGLES, 0, geo->element_count);
    }
}
}
}
