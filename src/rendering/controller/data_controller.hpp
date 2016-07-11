#pragma once
#include "rendering/model/model_base.hpp"
#include "rendering/controller/controller_base.hpp"
#include "rendering/abstract_user_input.hpp"
#include "rendering/release_renderer.hpp"
#include "data/world.hpp"
#include "rendering/model/star_model.hpp"
#include "rendering/model/hand_model.hpp"
#include "rendering/controller/caught_star_controller.hpp"
#include "../mesh.hpp"
#include <memory>
#include <map>

namespace cgvkp {
namespace rendering {
namespace controller {

    class data_controller : public controller_base {
    public:
        data_controller(release_renderer* renderer, const data::world& data, Mesh const& handMesh, Mesh const& starMesh);
        virtual ~data_controller();

        // Answer if the object has a model
		virtual inline bool has_model() const { return true; }

        /**
         * @param seconds Time elapsed since last call, in seconds
         */
        virtual void update(double seconds, std::shared_ptr<abstract_user_input> input);
    private:
        const ::cgvkp::data::world& data;
        std::map<unsigned int, std::weak_ptr<model::star_model> > stars;
        std::map<unsigned int, std::weak_ptr<model::hand_model> > hands;
        std::shared_ptr<caught_star_controller> caught_star_controller;
        release_renderer* renderer;
        // In debug hands have height: 0.1 and stars: 0.7 (see debug_user_input)
        // Transform that to a height from 0.4 to 2, which seems to an OK representation
        inline float trans_height(float h) { return ((h - 0.1f)/ 0.6f) * 1.6f + 0.4f; }

		Mesh const& handMesh;
		Mesh const& starMesh;
    };

}
}
}
