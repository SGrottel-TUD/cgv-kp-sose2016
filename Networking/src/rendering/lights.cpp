#include <glm/gtx/transform.hpp>
#include "lights.hpp"

void cgvkp::rendering::PointLight::calculateWorld()
{
	/*
	 * 0 <= itensity <= 1
	 * c = max(r, g, b) (0 <= r, g, b <= 1)
	 *
	 *     attenuation = constant + linear * distance + exponential * distance^2
	 * <=> exp * dist^2 + lin * dist + const - att = 0
	 * <=> distance = (-lin + sqrt(lin^2 - 4 * exp * (const - att))) / (2 * exp)
	 *
	 *     c * intensity / attenuation = threshold
	 * <=> attenuation = c * intensity / threshold
	 *  => distance = (-lin + sqrt(lin^2 - 4 * exp * (const - c * intensity / threshold))) / (2 * exp)
	*/

	float const threshold = 1.0f / 256;

	float maxColor = color.r > color.g ? color.r : color.g;
	maxColor = maxColor > color.b ? maxColor : color.b;

	float radius = (-linearAttenuation + sqrt(linearAttenuation * linearAttenuation - 4 * exponentialAttenuation * (constantAttenuation - maxColor * diffuseIntensity / threshold))) / (2 * exponentialAttenuation);

	world = glm::translate(position) * glm::scale(glm::vec3(radius));
}
