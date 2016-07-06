#pragma once
#include "data/world_config.hpp"
#include "data/input_layer.hpp"
#include "data/tripple_buffer.hpp"
#include <memory>
#include <vector>
#include "util/spline_curve.hpp"
#include <glm/glm.hpp>
#include <random>

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

        enum class game_mode {
            running,    // game is currently running
            paused,     // game is running but paused
            completed,  // game completed (e.g. show game over / highscores)
            stopped     // game is not running (e.g. show start screen / main menu)
        };

        world();
        ~world();

        void init();

        inline input_layer& get_input_layer() {
            return input;
        }

        inline world_config& get_config(void) {
            return cfg;
        }

        inline const world_config& get_config(void) const {
            return cfg;
        }

        /** Access to the hands */
        inline const std::vector<hand_ptr>& get_hands(void) const {
            return hands;
        }
        /** Access to the stars */
        inline const std::vector<star_ptr>& get_stars(void) const {
            return stars;
        }
        /** The current game score aka number of catched stars */
        inline unsigned int get_score(void) const {
            return score;
        }
        /**
         * Answer the current game mode.
         *
         * The world logic will work accordingly.
         */
        inline game_mode get_game_mode(void) const {
            return mode;
        }
        /**
         * Request to change the game mode
         *
         * The game mode will be changed as soon as possible. However, it is
         * not guaranteed to change instantly.
         */
        void set_game_mode(game_mode new_mode);

        /** merges data from the input layer */
        virtual void merge_input(void);
        /** updates the data assuming 1/60s time step */
        virtual void update_step(void);
    protected:
        std::vector<star_ptr> stars;
        std::vector<hand_ptr> hands;
        struct star_state {
            double t;
            double speed;
            util::spline_curve<glm::dvec3, double, 3> path;
        };
        typedef std::shared_ptr<star_state> star_state_ptr;
        std::vector<star_state_ptr> stars_states;

        unsigned int score;
    private:
        world_config cfg;
        tripple_buffer<input_layer::hand_collection> hands_input;
        tripple_buffer_facade<input_layer::hand_collection, 1> in_hands;
        input_layer input;

        struct hand_state {
            static const float retraction_accel;
            bool valid;
            float retract_speed;
        };
        typedef std::shared_ptr<hand_state> hand_state_ptr;
        std::vector<hand_state_ptr> hands_states;


        unsigned int next_hand_id;
        unsigned int next_star_id;
        game_mode mode;
        unsigned int star_spawn_timer;

        std::mt19937 rnd_engine;
        std::uniform_real_distribution<double> rnd_width;
        std::uniform_real_distribution<double> rnd_height;
        std::normal_distribution<double> rnd_elevation;
    };

}
}
