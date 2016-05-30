#include "rendering/technique_normal_as_colour.hpp"

cgvkp::rendering::TechniqueNormalAsColour::TechniqueNormalAsColour()
	: Technique()
{
}

bool cgvkp::rendering::TechniqueNormalAsColour::init()
{
	if (!Technique::init())
	{
		return false;
	}
	if (!addShader(GL_VERTEX_SHADER, "res/shaders/normal_as_colour.vs"))
	{
		return false;
	}
	if (!addShader(GL_FRAGMENT_SHADER, "res/shaders/normal_as_colour.fs"))
	{
		return false;
	}
	if (!link())
	{
		return false;
	}

	worldViewProjectionLocation = getUniformLocation("worldViewProjection");

	if (worldViewProjectionLocation == -1)
	{
		return false;
	}
    modelMatrixLocation = getUniformLocation("modelMatrix");

    if (modelMatrixLocation == -1)
    {
        return false;
    }

	return true;
}