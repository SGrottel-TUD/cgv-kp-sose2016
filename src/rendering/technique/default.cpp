#include "default.hpp"

bool cgvkp::rendering::DefaultTechnique::init()
{
	if (!Technique::init("default.vs", "default.fs"))
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

void cgvkp::rendering::DefaultTechnique::setWorldViewProjection(glm::mat4 const& worldViewProjection) const
{
	glUniformMatrix4fv(worldViewProjectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}
