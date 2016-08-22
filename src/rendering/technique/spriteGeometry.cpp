#include "spriteGeometry.hpp"

bool cgvkp::rendering::SpriteGeometryTechnique::init()
{
	if (!Technique::init("spriteGeometry.vs", "spriteGeometry.fs"))
	{
		return false;
	}

	projectionLocation = getUniformLocation("projection");

	if (projectionLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::SpriteGeometryTechnique::setProjection(glm::mat4 const& projection) const
{
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
}
