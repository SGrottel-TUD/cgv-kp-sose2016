#include "geometry.hpp"

bool cgvkp::rendering::GeometryTechnique::init()
{
	if (!Technique::init("geometry.vs", "geometry.fs"))
	{
		return false;
	}

	materialLocation = getUniformLocation("material");
	projectionLocation = getUniformLocation("projection");
	worldViewLocation = getUniformLocation("worldView");

	if (materialLocation == invalidLocation ||
		projectionLocation == invalidLocation ||
		worldViewLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::GeometryTechnique::setMaterial() const
{
	float specularPower = 1;
	float specularIntensity = 0;
	glUniform2f(materialLocation, specularPower, specularIntensity);
}

void cgvkp::rendering::GeometryTechnique::setProjection(glm::mat4 const& projection) const
{
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
}

void cgvkp::rendering::GeometryTechnique::setWorldView(glm::mat4 const& worldView) const
{
	glUniformMatrix4fv(worldViewLocation, 1, GL_FALSE, &worldView[0][0]);
}
