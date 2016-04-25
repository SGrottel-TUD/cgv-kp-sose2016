#pragma once

namespace cgvkp {
    class application_config {
    public:
        // Constructors
        application_config();
        application_config(int argc, char **argv);

        // enum classes
        enum class renderers : unsigned int {
            debug = 0u,
            release = 1u,
            models = 2u
        };
        enum class vision_inputs : unsigned int {
            debug = 0u,
            release = 1u,
            dummy = 2u
        };

        // Properties
        renderers active_renderer;
        vision_inputs active_vision;
    };
}
