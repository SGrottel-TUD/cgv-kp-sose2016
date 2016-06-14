#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "technique.hpp"

namespace cgvkp
{
    namespace rendering
    {
        class FontTechnique : public Technique
        {
        public:
			FontTechnique();
            virtual bool init();
        };
    }
}