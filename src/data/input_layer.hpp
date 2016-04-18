#pragma once
#include "data/world_config.hpp"
#include "data/tripple_buffer.hpp"
#include <vector>

namespace cgvkp {
namespace data {

    class input_layer {
    public:

        struct hand {
            /** Position in the world in meter */
            float x, y;
            /** Height of the hand over the game ground */
            float h;
        };

        typedef std::vector<hand> hand_collection;

        typedef tripple_buffer<hand_collection> input_hand_tripple_buffer;
        typedef tripple_buffer_facade<hand_collection, 0> input_hand_buffer;

        input_layer(const world_config& config, input_hand_buffer& buf);

        inline const world_config& get_config(void) const {
            return config;
        }

        /**
         * Access to the input buffer
         */
        inline hand_collection& buffer(void) {
            return buf.buffer();
        }

        /**
         * Synchronizes the input buffer with the data model.
         * Contents of buffer is undefined after this call and should be assumed invalid.
         */
        inline void sync_buffer(void) {
            buf.valid() = true; // sets the outgoing buffer to 'valid'
            buf.sync();
        }

    private:
        const world_config& config;
        input_hand_buffer buf;

    };

}
}
