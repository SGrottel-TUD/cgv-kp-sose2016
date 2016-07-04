#include "fontTechnique.hpp"

bool cgvkp::rendering::FontTechnique::init()
{
    if (!Technique::init())
    {
        return false;
    }
    if (!addShader(GL_VERTEX_SHADER, "shaders/font.vs"))
    {
        return false;
    }
    if (!addShader(GL_FRAGMENT_SHADER, "shaders/font.fs"))
    {
        return false;
    }
    if (!link())
    {
        return false;
    }

    worldViewProjectionLocation = getUniformLocation("worldViewProjection");

    if (worldViewProjectionLocation == invalidLocation)
    {
        return false;
    }

    return true;
}

void cgvkp::rendering::FontTechnique::setWorldViewProjection(glm::mat4 const& worldViewProjection) const
{
	glUniformMatrix4fv(worldViewProjectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}
