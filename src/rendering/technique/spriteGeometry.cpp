#include "spriteGeometry.hpp"

bool cgvkp::rendering::SpriteGeometryTechnique::init()
{
	if (!Technique::init("spriteGeometry.vs", "spriteGeometry.fs"))
	{
		return false;
	}

	projectionLocation = getUniformLocation("projection");
	materialLocation = getUniformLocation("material");
	GLint diffuseSamplerLocation = getUniformLocation("diffuseSampler");
	GLint depthOffsetSamplerLocation = getUniformLocation("depthOffsetSampler");


	if (materialLocation == invalidLocation ||
		projectionLocation == invalidLocation ||
		diffuseSamplerLocation == invalidLocation ||
		depthOffsetSamplerLocation == invalidLocation)
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

void cgvkp::rendering::SpriteGeometryTechnique::setProjection(glm::mat4 const& projection) const
{
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
}
