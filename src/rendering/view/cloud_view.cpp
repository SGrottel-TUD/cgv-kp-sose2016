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
				glBindVertexArray(geo->vao);
				glDrawElements(geo->indicesMode, geo->indicesCount, geo->indicesType, nullptr);
			}
		}
	}
}
