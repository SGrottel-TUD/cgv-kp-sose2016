#pragma once
#include "data/input_layer.hpp"
#include <memory>

namespace cgvkp {
namespace vision {

    class dummy_vision {
    public:
        dummy_vision(data::input_layer& input_layer);
        ~dummy_vision();

        bool init();
        void update();
        void deinit();

    private:
        data::input_layer& input_layer;
    };

}
}
