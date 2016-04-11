#pragma once
#include "data/world_config.h"
#include <memory>
#include <mutex>
#include <list>

namespace cgvkp {
namespace data {

    class input_layer {
    public:

        class hand {
        public:
            ~hand();

            inline unsigned int get_id(void) const {
                return id;
            }
            inline float get_x(void) const {
                return x;
            }
            inline float get_y(void) const {
                return y;
            }

            /** The height in meter */
            inline float get_height(void) const {
                return height;
            }
            void set_height(float h);

        private:
            hand(std::mutex& sync, unsigned int id, float x, float y);

            std::mutex& sync;
            unsigned int id;
            /** position and height in meter */
            float x, y, height;

            friend class ::cgvkp::data::input_layer;
        };

        typedef std::shared_ptr<hand> hand_ptr;

        input_layer(const world_config& config);
        ~input_layer();

        /** Answer the number of active hands in the data model */
        unsigned int hand_count();
        /**
         * Answer a hand in the data model
         *
         * @param idx Zero-based index
         */
        hand_ptr get_hand_from_index(unsigned int idx);
        /**
         * Answer a hand in the data model 
         *
         * @param id The hand id;
         */
        hand_ptr get_hand_from_id(unsigned int id);

        /**
         * Answer the hand at the specified position
         *
         * @param x The x coordinate in meter
         * @param y The y coordinate in meter
         * @param pos_eps Positional epsilon for searching the closes hand
         */
        hand_ptr find_hand_at(float x, float y, float pos_eps);
        inline hand_ptr find_hand_at(float x, float y) {
            return find_hand_at(x, y, config.positional_epsilon());
        }

        /**
         * Creates a new hand at the specified coordinates with height 0 
         *
         * If a hand on this location already exists
         */
        hand_ptr add_hand_at(float x, float y, float pos_eps);
        hand_ptr add_hand_at(float x, float y) {
            return add_hand_at(x, y, config.positional_epsilon());
        }

        /**
         * Sync methods are called by the framework to sync the data of the input layer into the data model.
         *
         * DO NOT CALL DIRECTLY!
         */
        void begin_sync();
        void end_sync();

    private:
        const world_config& config;
        std::mutex sync;
        unsigned int next_hand_id;
        std::list<hand_ptr> hands;

    };

}
}
