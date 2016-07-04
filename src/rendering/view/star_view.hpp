#pragma once
#include "rendering/model/star_model.hpp"
#include "view_base.hpp"

namespace cgvkp {
	namespace rendering {
		namespace view {
			class star_view : public view_base {
			public:
				typedef std::shared_ptr<star_view> ptr;

				star_view(Mesh const& mesh) : view_base(mesh) {}
				inline std::shared_ptr<model::star_model> get_model() const { return std::dynamic_pointer_cast<model::star_model>(model.lock()); }
			};
		}
	}
}
