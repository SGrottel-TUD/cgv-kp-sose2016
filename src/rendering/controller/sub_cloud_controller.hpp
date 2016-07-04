#pragma once
#include "rendering/model/model_base.hpp"
#include "rendering/controller/controller_base.hpp"
#include "rendering/abstract_user_input.hpp"
#include "rendering/release_renderer.hpp"
#include "data/world.hpp"
#include "rendering/model/cloud_model.hpp"
#include <map>
#include <memory>
#include <random>


namespace cgvkp {
namespace rendering {
namespace controller {

	class sub_cloud_controller : public controller_base {
	public:
		sub_cloud_controller(release_renderer* renderer, const data::world& data, Mesh const& mesh, std::shared_ptr<model::cloud_model> parent_cloud, float scale, float parent_distance);
		virtual ~sub_cloud_controller();

		// Answer if the object has a model
		virtual bool has_model() const;

		/**
		* @param seconds Time elapsed since last call, in seconds
		*/
		virtual void update(double seconds, std::shared_ptr<abstract_user_input> input);
	private:
		float parent_distance, parent_x;
		const ::cgvkp::data::world& data;
		std::shared_ptr<model::cloud_model> parent_cloud;
		std::shared_ptr<model::cloud_model> sub_cloud;
		release_renderer* renderer;
		std::default_random_engine random_engine;
		std::uniform_real_distribution<float> uniform;
};

}
}
}
