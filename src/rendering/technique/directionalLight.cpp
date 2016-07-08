#include "directionalLight.hpp"
#include "../geometryBuffer.hpp"
#include "../lights.hpp"

bool cgvkp::rendering::DirectionalLightTechnique::init()
{
	if (!Technique::init("quad.vs", "directionalLight.fs"))
	{
		return false;
	}

	diffuseColorLocation = getUniformLocation("light.diffuseColor");
	directionLocation = getUniformLocation("light.direction");
	GLint positionViewMapLocation = getUniformLocation("maps.positionView");
	GLint normalViewMapLocation = getUniformLocation("maps.normalView");
	GLint diffuseColorMapLocation = getUniformLocation("maps.diffuseColor");
	GLint materialMapLocation = getUniformLocation("maps.material");

	if (diffuseColorLocation == invalidLocation ||
		directionLocation == invalidLocation ||
		positionViewMapLocation == invalidLocation ||
		normalViewMapLocation == invalidLocation ||
		diffuseColorMapLocation == invalidLocation ||
		materialMapLocation == invalidLocation)
	{
		return false;
	}

	use();
	glUniform1i(positionViewMapLocation, GeometryBuffer::positionView);
	glUniform1i(normalViewMapLocation, GeometryBuffer::normalView);
	glUniform1i(diffuseColorMapLocation, GeometryBuffer::diffuseColor);
	glUniform1i(materialMapLocation, GeometryBuffer::material);

	return true;
}

void cgvkp::rendering::DirectionalLightTechnique::setLight(DirectionalLight const& light) const
{
	glUniform3f(directionLocation, light.direction.x, light.direction.y, light.direction.z);
	glm::vec3 color = light.color * light.diffuseIntensity;
	glUniform3f(diffuseColorLocation, color.x, color.y, color.z);
}
