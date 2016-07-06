#pragma once

#include "world.hpp"
#include <ostream>

namespace cgvkp {
    namespace data {

        struct message_header {
            float w;
            float h;
            float pos_eps;
            cgvkp::data::world::game_mode game_mode;
            uint32_t score;
            uint32_t stars_count;
        };
        inline std::ostream& operator<<(std::ostream& os, const message_header& header)
        {
            os << "score: " << header.score <<
                " stars: " << header.stars_count <<
                " w: " << header.w << " h: " << header.h << " eps" << header.pos_eps;
            return os;
        }

        class world_remote : public world {
        public:
            world_remote() : world() {}
            ~world_remote() {}

            /** Needs to be overriden to avoid problems. */
            inline virtual void merge_input(void)
            {
                stars_states.resize(stars.size());
                world::merge_input();
            }
            /** Irrelevant here */
            inline virtual void update_step(void) {};

            /** Direct access to the hands */
            inline std::vector<hand_ptr>& get_hands(void) {
                return hands;
            }
            /** Direct access to the stars */
            inline std::vector<star_ptr>& get_stars(void) {
                return stars;
            }
            inline void set_score(unsigned int new_score) {
                score = new_score;
            }
        };
    }
}
