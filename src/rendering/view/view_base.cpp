#include "rendering/view/view_base.hpp"

namespace cgvkp {
namespace rendering {
namespace view {
    bool view_base::init() {
        if (!initialized) {
            initialized = init_impl();
        }
        return initialized;
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