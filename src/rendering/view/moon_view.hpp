#pragma once
#include "rendering/model/moon_model.hpp"
#include "view_base.hpp"

namespace cgvkp
{
	namespace rendering
	{
		namespace view
		{
			class moon_view : public view_base
			{
			public:
				typedef std::shared_ptr<moon_view> ptr;

				moon_view(Mesh const& mesh) : view_base(mesh) {}
				inline std::shared_ptr<model::moon_model> get_model() const { return std::dynamic_pointer_cast<model::moon_model>(model.lock()); };
			};
		}
	}
}
