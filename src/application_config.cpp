#include "application_config.hpp"
#include <string.h>

cgvkp::application_config::application_config()
    : active_vision(vision_inputs::debug), active_renderer(renderers::debug) {
    // Intentionally empty
}

cgvkp::application_config::application_config(int argc, char **argv)
    : application_config::application_config() {
    for (int i = 0; i < argc; ++i) {
        if (strncmp(argv[i], "-rR", 3) == 0) {
            active_renderer = renderers::release;
        }
        if (strncmp(argv[i], "-rM", 3) == 0) {
            active_renderer = renderers::models;
        }
        if (strncmp(argv[i], "-vR", 3) == 0) {
            active_vision = vision_inputs::release;
        }
        if (strncmp(argv[i], "-vD", 3) == 0) {
            active_vision = vision_inputs::dummy;
        }
    }
}