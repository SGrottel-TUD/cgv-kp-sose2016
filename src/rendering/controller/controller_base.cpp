#include "rendering/controller/controller_base.hpp"

namespace cgvkp {
namespace rendering {
namespace controller {
    controller_base::controller_base() : model() {
        // intentionally empty
    }

    controller_base::~controller_base() {
        // intentionally empty
    }

    bool controller_base::has_model() const {
        return !model.expired();
    }
}
}
}
