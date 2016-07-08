#include "cloud_tech.hpp"

bool cgvkp::rendering::CloudTechnique::init()
{
	if (!Technique::init("cloud.vs", "cloud.fs"))
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

void cgvkp::rendering::CloudTechnique::setWorldViewProjection(glm::mat4 const& worldViewProjection) const
{
	glUniformMatrix4fv(worldViewProjectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}
