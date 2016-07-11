#include "spriteGeometry.hpp"

bool cgvkp::rendering::SpriteGeometryTechnique::init()
{
	if (!Technique::init("spriteGeometry.vs", "spriteGeometry.fs"))
	{
		return false;
	}

	materialLocation = getUniformLocation("material");
	worldViewLocation = getUniformLocation("worldView");
	worldViewProjectionLocation = getUniformLocation("worldViewProjection");

	if (materialLocation == invalidLocation ||
		worldViewLocation == invalidLocation ||
		worldViewProjectionLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::SpriteGeometryTechnique::setMaterial() const
{
	float specularPower = 1;
	float specularIntensity = 0;
	glUniform2f(materialLocation, specularPower, specularIntensity);
}

void cgvkp::rendering::SpriteGeometryTechnique::setWorldView(glm::mat4 const& worldView) const
{
	glUniformMatrix4fv(worldViewLocation, 1, GL_FALSE, &worldView[0][0]);
}

void cgvkp::rendering::SpriteGeometryTechnique::setWorldViewProjection(glm::mat4 const& worldViewProjection) const
{
	glUniformMatrix4fv(worldViewProjectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}
