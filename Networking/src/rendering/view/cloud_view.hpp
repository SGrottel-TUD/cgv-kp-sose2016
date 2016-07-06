#pragma once
#include "rendering/model/cloud_model.hpp"
#include "rendering/view/cloud_geometry.hpp"
#include "rendering/view/view_geometry_base.hpp"
namespace cgvkp {
	namespace rendering {
		namespace view {
			class cloud_view : public view_geometry_base<cloud_geometry> {
			public:
				cloud_view();
				virtual ~cloud_view();
				virtual void render();
				std::shared_ptr<model::cloud_model> get_model() const;
			};
		}
	}
}