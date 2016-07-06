#include "rendering/view/star_view.hpp"

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
		glBindVertexArray(geo->vao);
        geo->texture->bind_texture();
		glDrawElements(geo->indicesMode, geo->indicesCount, geo->indicesType, nullptr);
	}
}
}
}
