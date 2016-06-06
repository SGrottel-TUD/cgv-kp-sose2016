#pragma once
#include "rendering/model/model_base.hpp"
#include "rendering/controller/controller_base.hpp"
#include "rendering/abstract_user_input.hpp"
#include "rendering/release_renderer.hpp"
#include "rendering/model/cloud_model.hpp"
#include <map>
#include <memory>

namespace cgvkp {
namespace rendering {
namespace controller {

    class cloud_controller : public controller_base {
    public:
        cloud_controller(release_renderer* renderer);
        virtual ~cloud_controller();

        // Answer if the object has a model
        virtual bool has_model() const;

        /**
         * @param seconds Time elapsed since last call, in seconds
         */
        virtual void update(double seconds, std::shared_ptr<abstract_user_input> input);
    private:
        std::map<unsigned int, std::weak_ptr<model::cloud_model> > clouds;
        release_renderer* renderer;
    };

}
}
}
