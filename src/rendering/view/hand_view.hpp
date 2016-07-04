#pragma once
#include "rendering/model/hand_model.hpp"
#include "view_base.hpp"

namespace cgvkp
{
	namespace rendering
	{
		namespace view
		{
			class hand_view : public view_base
			{
			public:
				typedef std::shared_ptr<hand_view> ptr;

				hand_view(Mesh const& mesh) : view_base(mesh) {}
				inline std::shared_ptr<model::hand_model> get_model() const { return std::dynamic_pointer_cast<model::hand_model>(model.lock()); };
			};
		}
	}
}