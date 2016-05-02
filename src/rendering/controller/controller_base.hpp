#pragma once
#include "rendering/model/model_base.hpp"
#include "rendering/abstract_user_input.hpp"
#include <memory>

namespace cgvkp {
namespace rendering {
namespace controller {

    class controller_base {
    public:
        typedef std::shared_ptr<controller_base> ptr;
        typedef std::weak_ptr<controller_base> weak_ptr;

        controller_base();
        virtual ~controller_base();

        inline void set_model(model::model_base::weak_ptr m) {
            model = m;
        }

        // Answer if the object has a model
        virtual bool has_model() const;

        /**
         * @param seconds Time elapsed since last call, in seconds
         */
        virtual void update(double seconds, const abstract_user_input& input) = 0;

    protected:
        /** The main model used by this controller */
        model::model_base::weak_ptr model;
    };

}
}
}
