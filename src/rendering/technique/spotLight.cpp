#include <glm/gtx/transform.hpp>
#include "spotLight.hpp"
#include "../geometryBuffer.hpp"
#include "../lights.hpp"

bool cgvkp::rendering::SpotLightTechnique::init()
{
	if (!Technique::init("quad.vs", "spotLight.fs"))
	{
		return false;
	}

	projectionLocation = getUniformLocation("projection");
	lightLocation.color = getUniformLocation("light.color");
	lightLocation.position = getUniformLocation("light.position");
	lightLocation.direction = getUniformLocation("light.direction");
	lightLocation.attenuation = getUniformLocation("light.attenuation");
	lightLocation.cutoff = getUniformLocation("light.cutoff");
	GLint positionViewMapLocation = getUniformLocation("maps.positionView");
	GLint normalViewMapLocation = getUniformLocation("maps.normalView");
	GLint diffuseColorMapLocation = getUniformLocation("maps.diffuseColor");

	if (projectionLocation == invalidLocation ||
		lightLocation.position == invalidLocation ||
		lightLocation.color == invalidLocation ||
		lightLocation.attenuation == invalidLocation ||
		positionViewMapLocation == invalidLocation ||
		normalViewMapLocation == invalidLocation ||
		diffuseColorMapLocation == invalidLocation)
	{
		return false;
	}

	use();
	glUniform1i(positionViewMapLocation, GeometryBuffer::positionView);
	glUniform1i(normalViewMapLocation, GeometryBuffer::normalView);
	glUniform1i(diffuseColorMapLocation, GeometryBuffer::diffuseColor);

	return true;
}

void cgvkp::rendering::SpotLightTechnique::setLight(SpotLight const& light) const
{
	glUniform3f(lightLocation.position, light.position.x, light.position.y, light.position.z);
	glUniform3f(lightLocation.direction, light.direction.x, light.direction.y, light.direction.z);
	glm::vec3 color = light.color * light.diffuseIntensity;
	glUniform3f(lightLocation.color, color.x, color.y, color.z);
	glUniform3f(lightLocation.attenuation, light.attenuation.x, light.attenuation.y, light.attenuation.z);
	glUniform1f(lightLocation.cutoff, cos(light.cutoff));
}

void cgvkp::rendering::SpotLightTechnique::setWorldViewProjection(glm::mat4 const& worldViewProjection) const
{
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}
