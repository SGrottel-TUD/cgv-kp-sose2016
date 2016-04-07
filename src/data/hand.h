#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "data/star.h"

namespace cgvkp {
namespace data {

    class world;

    class hand {
    public:
        typedef std::shared_ptr<hand> ptr_type;
        friend class ::cgvkp::data::world;

        ~hand();

        unsigned int get_id(void) const {
            return id;
        }
        glm::vec2& position(void) {
            return pos;
        }
        const glm::vec2& position(void) const {
            return pos;
        }
        float get_height(void) const {
            return height;
        }
        void set_height(float h) {
            height = h;
        }
        star::ptr_type get_captured_star(void) const {
            return captured;
        }
        void set_captured_star(star::ptr_type s) {
            captured = s;
        }

    private:
        hand();

        unsigned int id; // unique identifier
        glm::vec2 pos; // position
        float height; // height of the hand over the game ground
        star::ptr_type captured; // the captured star
    };

}
}
