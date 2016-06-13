#include "rendering/view/cloud_view.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace cgvkp {
	namespace rendering {
		namespace view {
			cloud_view::cloud_view() : view_geometry_base<cloud_geometry>() {
			}
			cloud_view::~cloud_view() {
			}
			std::shared_ptr<model::cloud_model> cloud_view::get_model() const {
				return std::dynamic_pointer_cast<model::cloud_model>(model.lock());
			}
			void cloud_view::render() {
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
