#pragma once
#include "rendering/model/cloud_model.hpp"
#include "view_base.hpp"

namespace cgvkp
{
	namespace rendering
	{
		namespace view
		{
			class cloud_view : public view_base
			{
			public:
				typedef std::shared_ptr<cloud_view> ptr;

				inline cloud_view(Mesh const& mesh) : view_base(mesh) {}
				inline std::shared_ptr<model::cloud_model> get_model() const { return std::dynamic_pointer_cast<model::cloud_model>(model.lock()); };
			};
		}
	}
}
