#pragma once

#include "technique.hpp"

namespace cgvkp
{
    namespace rendering
    {
        class technique_normal_as_colour : public Technique
        {
        public:
            technique_normal_as_colour();
			void setModelMatrix(glm::mat4x4 const& modelMatrix) const;
            virtual bool init();
            static std::string get_id() { return "normal_as_colour"; }
        private:
            GLint modelMatrixLocation;
        };
    }
}