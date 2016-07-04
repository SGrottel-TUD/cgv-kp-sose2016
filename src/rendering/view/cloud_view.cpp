#include "rendering/view/cloud_view.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace cgvkp {
	namespace rendering {
		namespace view {
			void cloud_view::render() const {
				glBindVertexArray(geo->vao);
                geo->texture->bind_texture();
				glDrawElements(geo->indicesMode, geo->indicesCount, geo->indicesType, nullptr);
			}
		}
	}
}
