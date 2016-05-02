#include "rendering/view/view_base.hpp"
#include <GL/glew.h>
#include <iostream>
#include <cassert>

namespace cgvkp {
namespace rendering {
namespace view {
    view_base::view_base() : model(), initialized(false) {
        // intentionally empty
    }

    view_base::~view_base() {
        assert(!initialized);
    }

    bool view_base::init() {
        if (!initialized) {
            initialized = init_impl();
        }
        return initialized;
    }

    bool view_base::is_valid() const {
        return initialized;
    }

    bool view_base::has_model() const {
        return !model.expired();
    }

    void view_base::deinit() {
        if (initialized) {
            deinit_impl();
            initialized = false;
        }
    }
}
}
}