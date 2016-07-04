#pragma once

#include <glm/mat4x4.hpp>
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
			void setWorldViewProjection(glm::mat4x4 const& worldViewProjection) const;
            virtual bool init();

        private:
            GLint modelMatrixLocation;
			GLint worldViewProjectionLocation;
        };
    }
}