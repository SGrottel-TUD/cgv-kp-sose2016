#include "rendering/view/star_view.hpp"

namespace cgvkp {
namespace rendering {
namespace view {
	void star_view::render() const {
		glBindVertexArray(geo->vao);
        geo->texture->bind_texture();
		glDrawElements(geo->indicesMode, geo->indicesCount, geo->indicesType, nullptr);
	}
}
}
}
