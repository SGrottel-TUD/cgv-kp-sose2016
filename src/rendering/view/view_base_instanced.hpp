#pragma once
#include "rendering/model/model_base.hpp"
#include "../mesh.hpp"
#include <memory>
#include <vector>

namespace cgvkp
{
	namespace rendering
	{
		namespace view
		{
			class view_base {
			public:
				typedef std::shared_ptr<view_base> ptr;
				typedef std::weak_ptr<view_base> weak_ptr;

				view_base(Mesh const& m) : mesh(m) {}

				inline void set_model(model::model_base::weak_ptr m) { model = m; }
				inline model::model_base::ptr get_model() const { return model.lock(); }

				// Answer if the object has a model
				inline bool has_model() const { return !model.expired(); }

				inline void render() const { mesh.render(); }

			protected:

				/** The list of models used by this view */
				std::vector<model::model_base::weak_ptr> model;

			private:
				Mesh const& mesh;
			};

		}
	}
}
