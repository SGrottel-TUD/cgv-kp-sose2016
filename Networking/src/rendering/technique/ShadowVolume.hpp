#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "technique.hpp"

namespace cgvkp
{
    namespace rendering
    {
        class ShadowVolumeTechnique : public Technique
        {
        public:
            ShadowVolumeTechnique();
            virtual bool init();

            void setLightPosition(glm::vec3 const& lightPosition) const;
            void setViewProjection(glm::mat4x4 const& viewProjection) const;
            void setWorld(glm::mat4x4 const& world) const;
            bool renderView(view::view_base::ptr const view) const;

        private:
            GLint lightPositionLocation;
            GLint viewProjectionLocation;
            GLint worldLocation;
        };
    }
}