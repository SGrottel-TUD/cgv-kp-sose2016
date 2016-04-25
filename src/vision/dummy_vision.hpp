#pragma once
#include "vision/abstract_vision.hpp"
#include <memory>

namespace cgvkp {
namespace vision {

    class dummy_vision : public abstract_vision {
    public:
        dummy_vision(data::input_layer& input_layer);
        virtual ~dummy_vision();

        virtual void update();
    protected:
        bool init_impl();
        void deinit_impl();
    };

}
}
