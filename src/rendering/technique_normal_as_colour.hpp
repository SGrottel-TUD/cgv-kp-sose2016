#pragma once

#include "technique.hpp"

namespace cgvkp
{
    namespace rendering
    {
        class TechniqueNormalAsColour : public Technique
        {
        public:
            TechniqueNormalAsColour();
			void setModelMatrix(glm::mat4x4 const& modelMatrix) const;
            virtual bool init();
        private:
            GLint modelMatrixLocation;
        };
    }
}