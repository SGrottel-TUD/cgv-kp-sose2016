#include <glm/gtx/transform.hpp>
#include <random>
#include "ssao.hpp"
#include "../geometryBuffer.hpp"

bool cgvkp::rendering::SSAOTechnique::init()
{
	if (!Technique::init())
	{
		return false;
	}
	if (!addShader(GL_VERTEX_SHADER, "shaders/quad.vs"))
	{
		return false;
	}
	if (!addShader(GL_FRAGMENT_SHADER, "shaders/ssao.fs"))
	{
		return false;
	}
	if (!link())
	{
		return false;
	}

	ambientLightLocation = getUniformLocation("ambientLight");
	GLint positionViewMap = getUniformLocation("maps.positionView");
	GLint normalViewMap = getUniformLocation("maps.normalView");
	GLint diffuseColorMap = getUniformLocation("maps.diffuseColor");
	GLint randomDirectionsLocation = getUniformLocation("randomDirections");

	if (ambientLightLocation == invalidLocation ||
		positionViewMap == invalidLocation ||
		normalViewMap == invalidLocation ||
		diffuseColorMap == invalidLocation ||
		randomDirectionsLocation == invalidLocation)
	{
		return false;
	}

	use();
	glUniform1i(positionViewMap, GeometryBuffer::positionView);
	glUniform1i(normalViewMap, GeometryBuffer::normalView);
	glUniform1i(diffuseColorMap, GeometryBuffer::diffuseColor);

	// Set random directions uniform
	int const numRandomDirections = 6;
	glm::vec3 randomDirections[numRandomDirections];
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0.0, 1.0);
	for (int i = 0; i < numRandomDirections; ++i)
	{
		randomDirections[i] = glm::normalize(glm::vec3(distribution(generator), distribution(generator), distribution(generator)));
	}
	randomDirections[0] = glm::vec3(1, 0, 0);	// something weird is going on with the directions in the shader... Will be deleted in the future.
	randomDirections[1] = glm::vec3(-1, 0, 0);
	randomDirections[2] = glm::vec3(0, 1, 0);
	randomDirections[3] = glm::vec3(0, -1, 0);
	randomDirections[4] = glm::vec3(0, 0, 1);
	randomDirections[5] = glm::vec3(0, 0, -1);
	glUniform3fv(randomDirectionsLocation, numRandomDirections, &(randomDirections[0][0]));

	return true;
}

void cgvkp::rendering::SSAOTechnique::setAmbientLight(glm::vec3 const& color) const
{
	glUniform3f(ambientLightLocation, color.x, color.y, color.z);
}
