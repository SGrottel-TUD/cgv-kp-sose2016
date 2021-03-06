#pragma once
#include "data/world_config.h"
#include "data/input_layer.h"
#include "data/tripple_buffer.h"
#include <memory>
#include <vector>

namespace cgvkp {
namespace data {

	class world {
	public:

        struct hand;
        struct star;
        typedef std::shared_ptr<hand> hand_ptr;
        typedef std::shared_ptr<star> star_ptr;

        struct hand {
            unsigned int id;    // unique hand id
            float x, y;         // position (meter)
            float height;       // height of hand over ground (meter)
            star_ptr star;      // pointer to the star in hand (or nullptr)
        };

        struct star {
            unsigned int id;    // unique star id
            float x, y;         // position (meter)
            float height;       // height of star over ground (meter)
            float dx, dy;       // movement direction
            bool in_hand;       // true if star is captured in a hand
        };

		world();
		~world();

        void init();

        inline input_layer& get_input_layer() {
            return input;
        }

        /** Access to the hands */
        inline const std::vector<hand_ptr>& get_hands(void) const {
            return hands;
        }
        /** Access to the stars */
        inline const std::vector<star_ptr>& get_stars(void) const {
            return stars;
        }

        /** merges data from the input layer */
        void merge_input(void);
        /** updates the data assuming 1/60s time step */
        void update_step(void);

    private:
        world_config cfg;
        tripple_buffer<input_layer::hand_collection> hands_input;
        tripple_buffer_facade<input_layer::hand_collection, 1> in_hands;
        input_layer input;

        struct star_state {
            // TODO: more to come
        };
        typedef std::shared_ptr<star_state> star_state_ptr;

        std::vector<hand_ptr> hands;
        std::vector<star_ptr> stars;
        std::vector<star_state_ptr> stars_states;

        unsigned int score;
        unsigned int next_hand_id;
        unsigned int next_star_id;
	};

}
}
