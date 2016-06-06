#pragma once
#include "rendering/model/model_base.hpp"
#include "rendering/controller/controller_base.hpp"
#include "rendering/abstract_user_input.hpp"
#include "rendering/release_renderer.hpp"
#include "data/world.hpp"
#include "rendering/model/star_model.hpp"
#include "rendering/model/hand_model.hpp"
#include "rendering/controller/caught_star_controller.hpp"
#include <memory>
#include <map>

namespace cgvkp {
namespace rendering {
namespace controller {

    class data_controller : public controller_base {
    public:
        data_controller(release_renderer* renderer, const data::world& data);
        virtual ~data_controller();

        // Answer if the object has a model
        virtual bool has_model() const;

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
    };

}
}
}
