#pragma once
#include "data/input_layer.hpp"

namespace cgvkp {
namespace data {
    class world;
}
namespace vision {

    class abstract_vision {
    public:
        abstract_vision(data::input_layer& input_layer);
        virtual ~abstract_vision();
        // Initializes the object (if not already initialized) and returns the initialization state
        bool init();
        // deinitializes the object (if initialized)
        void deinit();
        // Do updates
        virtual void update() = 0;
    protected:
        // return true on success
        virtual bool init_impl() = 0;
        virtual void deinit_impl() = 0;

        data::input_layer& input_layer;
    private:
        bool initialized;
    };

}
}
