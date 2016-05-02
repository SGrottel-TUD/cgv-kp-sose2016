#include "rendering/view/star_view.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace cgvkp {
namespace rendering {
namespace view {
    star_view::star_view() : view_geometry_base<star_geometry>() {
    }
    star_view::~star_view() {
    }
    std::shared_ptr<model::star_model> star_view::get_model() const {
        return std::dynamic_pointer_cast<model::star_model>(model.lock());
    }
    void star_view::render() {
        auto m = get_model();
        ::glDisable(GL_CULL_FACE);
        ::glEnable(GL_DEPTH_TEST);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glDepthMask(GL_TRUE);;

        ::glUseProgram(geo->shader);

        ::glBindVertexArray(geo->vao);

        glm::mat4 view_matrix = glm::mat4(1.0f);
        glm::mat4 projection_matrix = glm::mat4(1.0f);

        ::glUniformMatrix4fv(geo->model_loc, 1, false, glm::value_ptr(m->model_matrix));
        ::glUniformMatrix4fv(geo->view_loc, 1, false, glm::value_ptr(view_matrix));
        ::glUniformMatrix4fv(geo->projection_loc, 1, false, glm::value_ptr(projection_matrix));

        ::glDrawElements(GL_TRIANGLES, geo->element_count, GL_UNSIGNED_INT, nullptr);
    }
}
}
}
