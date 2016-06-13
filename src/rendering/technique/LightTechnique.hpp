#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "technique.hpp"
#include "rendering/lights.hpp"

namespace cgvkp
{
    namespace rendering
    {
        class LightTechnique : public Technique
        {
        public:
            LightTechnique();
            virtual bool init();
            void setEyePosition(GLfloat x, GLfloat y, GLfloat z) const;
            void setLight(PointLight const& light) const;
            void setMaps() const;
            void setScreenSize(GLsizei width, GLsizei height) const;

        private:
            struct
            {
                GLint position;
                GLint diffuseColor;
                GLint attenuation;
            } lightLocation;
            struct
            {
                GLint position;
                GLint normal;
                GLint diffuseColor;
                GLint material;
            } mapLocation;
            GLint eyePositionLocation;
            GLint screenSizeLocation;
        };
    }
}