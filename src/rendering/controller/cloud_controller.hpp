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

    class cloud_controller : public controller_base {
    public:
        cloud_controller(release_renderer* renderer, const data::world& data);
        virtual ~cloud_controller();

        // Answer if the object has a model
        virtual bool has_model() const;

        /**
         * @param seconds Time elapsed since last call, in seconds
         */
        virtual void update(double seconds, std::shared_ptr<abstract_user_input> input);
    private:

		float calculate_max_cloud_space(float z);
		std::vector<float> calculate_new_speed_curve();

		float w, h;
		const ::cgvkp::data::world& data;
        std::map<unsigned int, std::weak_ptr<model::cloud_model> > clouds;
        release_renderer* renderer;
		std::default_random_engine random_engine;
		std::uniform_real_distribution<float> uniform;
		std::uniform_int_distribution<int> int_uniform;

    };

}
}
}
