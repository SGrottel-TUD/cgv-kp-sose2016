#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include "technique.hpp"

namespace cgvkp
{
    namespace rendering
    {
        class FontTechnique : public Technique
        {
        public:
			FontTechnique() : worldViewProjectionLocation(invalidLocation) {}
            virtual bool init();
			void setWorldViewProjection(glm::mat4 const& worldViewProjection) const;

		private:
			GLint worldViewProjectionLocation;
        };
    }
}
