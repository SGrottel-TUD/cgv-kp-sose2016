#include "rendering/view/hand_view.hpp"

namespace cgvkp {
namespace rendering {
namespace view {
    std::vector<std::string> hand_view::_techniques({ "Geometry", "ShadowVolume" });
	hand_view::hand_view() : view_geometry_base<hand_geometry>() {
	}
	hand_view::~hand_view() {
	}
	std::shared_ptr<model::hand_model> hand_view::get_model() const {
		return std::dynamic_pointer_cast<model::hand_model>(model.lock());
	}
	void hand_view::render() {
		glBindVertexArray(geo->vao);
		glDrawElements(geo->indicesMode, geo->indicesCount, geo->indicesType, nullptr);
	}
}
}
}
