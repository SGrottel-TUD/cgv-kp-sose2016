#pragma once
#include "rendering/model/star_model.hpp"
#include "rendering/view/star_geometry.hpp"
#include "rendering/view/view_geometry_base.hpp"

namespace cgvkp {
namespace rendering {
namespace view {
	class star_view : public view_geometry_base<star_geometry> {
	public:
		virtual void render() const;
		inline std::shared_ptr<model::star_model> get_model() const { return std::dynamic_pointer_cast<model::star_model>(model.lock()); }
		virtual inline bool light_source() { return true; }
	};
}
}
}