#include<memory>

#include "LightTechnique.hpp"
#include "geometryBuffer.hpp"
#include "rendering/model/graphic_model_base.hpp"

cgvkp::rendering::LightTechnique::LightTechnique()
    : mapLocation{ invalidLocation, invalidLocation, invalidLocation, invalidLocation }, lightLocation{ invalidLocation, invalidLocation, invalidLocation },
    eyePositionLocation(invalidLocation), screenSizeLocation(invalidLocation)
{
}

bool cgvkp::rendering::LightTechnique::init()
{
    if (!Technique::init())
    {
        return false;
    }
    if (!addShader(GL_VERTEX_SHADER, "shaders/light.vs"))
    {
        return false;
    }
    if (!addShader(GL_FRAGMENT_SHADER, "shaders/light.fs"))
    {
        return false;
    }
    if (!link())
    {
        return false;
    }

    mapLocation.position = getUniformLocation("maps.position");
    mapLocation.normal = getUniformLocation("maps.normal");
    mapLocation.diffuseColor = getUniformLocation("maps.diffuseColor");
    mapLocation.material = getUniformLocation("maps.material");

    lightLocation.position = getUniformLocation("light.position");
    lightLocation.diffuseColor = getUniformLocation("light.diffuseColor");
    lightLocation.attenuation = getUniformLocation("light.attenuation");

    eyePositionLocation = getUniformLocation("eyePosition");
    screenSizeLocation = getUniformLocation("screenSize");
    worldViewProjectionLocation = getUniformLocation("worldViewProjection");

    if (mapLocation.position == invalidLocation ||
        mapLocation.normal == invalidLocation ||
        mapLocation.diffuseColor == invalidLocation ||
        mapLocation.material == invalidLocation ||

        lightLocation.position == invalidLocation ||
        lightLocation.diffuseColor == invalidLocation ||
        lightLocation.attenuation == invalidLocation ||

        eyePositionLocation == invalidLocation ||
        screenSizeLocation == invalidLocation ||
        worldViewProjectionLocation == invalidLocation)
    {
        return false;
    }

    return true;
}

void cgvkp::rendering::LightTechnique::setEyePosition(GLfloat x, GLfloat y, GLfloat z) const
{
    glUniform3f(eyePositionLocation, x, y, z);
}

void cgvkp::rendering::LightTechnique::setLight(PointLight const& light) const
{
    glUniform3f(lightLocation.position, light.position.x, light.position.y, light.position.z);
    glUniform3f(lightLocation.diffuseColor, light.color.r * light.diffuseIntensity, light.color.g * light.diffuseIntensity, light.color.b * light.diffuseIntensity);
    glUniform3f(lightLocation.attenuation, light.constantAttenuation, light.linearAttenuation, light.exponentialAttenuation);
}

void cgvkp::rendering::LightTechnique::setMaps() const
{
    glUniform1i(mapLocation.position, GeometryBuffer::position);
    glUniform1i(mapLocation.normal, GeometryBuffer::normal);
    glUniform1i(mapLocation.diffuseColor, GeometryBuffer::diffuseColor);
    glUniform1i(mapLocation.material, GeometryBuffer::material);
}

void cgvkp::rendering::LightTechnique::setScreenSize(GLsizei width, GLsizei height) const
{
    glUniform2i(screenSizeLocation, width, height);
}
