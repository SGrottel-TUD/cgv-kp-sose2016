#include "background.hpp"

bool cgvkp::rendering::BackgroundTechnique::init()
{
	if (!Technique::init("geometry.vs", "background.fs"))
	{
		return false;
	}

	normalLocation = getUniformLocation("normal");
	projectionLocation = getUniformLocation("projection");
	worldViewLocation = getUniformLocation("worldView");

	if (normalLocation == invalidLocation ||
		projectionLocation == invalidLocation ||
		worldViewLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::BackgroundTechnique::setNormal(glm::vec3 const& normal) const
{
	glUniform3f(normalLocation, normal.x, normal.y, normal.z);
}

void cgvkp::rendering::BackgroundTechnique::setProjection(glm::mat4 const& projection) const
{
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
}

void cgvkp::rendering::BackgroundTechnique::setWorldView(glm::mat4 const& worldView) const
{
	glUniformMatrix4fv(worldViewLocation, 1, GL_FALSE, &worldView[0][0]);
}
