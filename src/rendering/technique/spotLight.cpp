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
	GLint materialMapLocation = getUniformLocation("maps.material");

	if (projectionLocation == invalidLocation ||
		lightLocation.position == invalidLocation ||
		lightLocation.color == invalidLocation ||
		lightLocation.attenuation == invalidLocation ||
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

void cgvkp::rendering::SpotLightTechnique::setLight(SpotLight const& light, glm::mat4 const& view, glm::mat4 const& projection) const
{
	float distance = light.calculateMaxDistance();
	glm::vec3 positionView(view * glm::vec4(light.position, 1));
	glm::vec3 directionView(view * glm::vec4(light.direction, 0));

	// There are 3 points which approximately define a aabb for the light: the position, and an aabb around position + distance * direction which is a square with the length of distance * sin(cutoff / 2).
	glm::vec4 positionProjected = projection * glm::vec4(positionView, 1);
	glm::vec4 directionProjected = projection * glm::vec4(directionView, 0);
	glm::vec4 aabbProjected = projection * glm::vec4(sin(light.cutoff / 2), sin(light.cutoff / 2), 0, 0);
	glm::vec4 bottomLeft = positionProjected + distance * (directionProjected - aabbProjected);
	glm::vec4 topRight = positionProjected + distance * (directionProjected + aabbProjected);

	positionProjected /= positionProjected.w;
	bottomLeft /= bottomLeft.w;
	topRight /= topRight.w;

	glm::vec2 pos(positionProjected);
	glm::vec2 min(bottomLeft);
	glm::vec2 max(topRight);

	if (positionProjected.x < min.x)
	{
		min.x = positionProjected.x;
	}
	if (positionProjected.y < min.y)
	{
		min.y = positionProjected.y;
	}
	if (positionProjected.x > max.x)
	{
		max.x = positionProjected.x;
	}
	if (positionProjected.y > max.y)
	{
		max.y = positionProjected.y;
	}

	glm::mat4 quadProjection = glm::translate(glm::vec3(positionProjected.x, positionProjected.y, 0)) * glm::scale(glm::vec3(0.5f * (max - min), 1)) * glm::translate(glm::vec3(0, -1, 0));

	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &quadProjection[0][0]);
	glUniform3f(lightLocation.position, positionView.x, positionView.y, positionView.z);
	glUniform3f(lightLocation.direction, directionView.x, directionView.y, directionView.z);
	glm::vec3 color = light.color * light.diffuseIntensity;
	glUniform3f(lightLocation.color, color.x, color.y, color.z);
	glUniform3f(lightLocation.attenuation, light.attenuation.x, light.attenuation.y, light.attenuation.z);
	glUniform1f(lightLocation.cutoff, cos(light.cutoff));
}
