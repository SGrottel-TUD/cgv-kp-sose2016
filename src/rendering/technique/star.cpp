#include "star.hpp"

bool cgvkp::rendering::StarTechnique::init()
{
	if (!Technique::init("star.vs", "star.fs"))
	{
		return false;
	}

	textureCoordOffsetLocation = getUniformLocation("textureCoordOffset");
	worldViewProjectionLocation = getUniformLocation("worldViewProjection");

	if (textureCoordOffsetLocation == invalidLocation ||
		worldViewProjectionLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::StarTechnique::setTextureCoordOffset(glm::vec2 const& offset) const
{
	glUniform2f(textureCoordOffsetLocation, offset.x, offset.y);
}

void cgvkp::rendering::StarTechnique::setWorldViewProjection(glm::mat4 const& worldViewProjection) const
{
	glUniformMatrix4fv(worldViewProjectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}
