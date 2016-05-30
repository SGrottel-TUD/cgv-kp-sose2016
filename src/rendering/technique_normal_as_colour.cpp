#include "technique_normal_as_colour.hpp"

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
void cgvkp::rendering::TechniqueNormalAsColour::setModelMatrix(glm::mat4x4 const& modelMatrix) const
{
	if (modelMatrixLocation != -1)
	{
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
	}
}