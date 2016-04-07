#pragma once
#include <memory>
#include "glm/glm.hpp"

namespace cgvkp {
namespace data {

    class world;

    class star {
    public:
        typedef std::shared_ptr<star> ptr_type;
        friend class ::cgvkp::data::world;

        ~star();

        unsigned int get_id(void) const {
            return id;
        }
        glm::vec2& position(void) {
            return pos;
        }
        const glm::vec2& position(void) const {
            return pos;
        }
        glm::vec2& direction(void) {
            return dir;
        }
        const glm::vec2& direction(void) const {
            return dir;
        }
        bool is_captured(void) const {
            return captured;
        }
        void set_captured(bool cap) {
            captured = cap;
        }

    private:

        star();

        unsigned int id; // unique identifier
        glm::vec2 pos; // position
        glm::vec2 dir; // direction (viewing and moving)
        bool captured; // flag if this star is captured by a hand
    };

}
}
