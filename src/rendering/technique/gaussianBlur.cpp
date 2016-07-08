#include "gaussianBlur.hpp"
#include "../geometryBuffer.hpp"

bool cgvkp::rendering::GaussianBlurTechnique::init()
{
	if (!Technique::init("quad.vs", "gaussianBlur.fs"))
	{
		return false;
	}

	blurSamplerLocation = getUniformLocation("blurSampler");
	blurSizeLocation = getUniformLocation("blurSize");
	directionLocation = getUniformLocation("direction");

	if (blurSamplerLocation == invalidLocation ||
		blurSizeLocation == invalidLocation ||
		directionLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::GaussianBlurTechnique::setBlurSize(GLsizei size) const
{
	glUniform1f(blurSizeLocation, 1.0f / size);
}

void cgvkp::rendering::GaussianBlurTechnique::setDirection(Direction direction) const
{
	if (direction == horizontal)
	{
		glUniform2f(directionLocation, 1.0f, 0.0f);
	}
	else
	{
		glUniform2f(directionLocation, 0.0f, 1.0f);
	}
}
