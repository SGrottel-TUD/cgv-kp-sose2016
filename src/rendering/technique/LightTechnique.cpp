#include "lightTechnique.hpp"
#include "../geometryBuffer.hpp"
#include "rendering/model/graphic_model_base.hpp"

cgvkp::rendering::LightTechnique::LightTechnique()
	: eyePositionLocation(invalidLocation), screenSizeLocation(invalidLocation), worldViewProjectionLocation(invalidLocation)
{
	mapLocation.diffuseColor = invalidLocation; mapLocation.material = invalidLocation;
	mapLocation.normalWorld = invalidLocation; mapLocation.positionWorld = invalidLocation;
	lightLocation.attenuation = invalidLocation; lightLocation.diffuseColor = invalidLocation;
	lightLocation.positionWorld = invalidLocation;
}

bool cgvkp::rendering::LightTechnique::init()
{
	if (!Technique::init())
	{
		return false;
	}
	if (!addShader(GL_VERTEX_SHADER, "shaders/light.vs"))
	{
		return false;
	}
	if (!addShader(GL_FRAGMENT_SHADER, "shaders/light.fs"))
	{
		return false;
	}
	if (!link())
	{
		return false;
	}

	mapLocation.positionWorld = getUniformLocation("maps.positionWorld");
	mapLocation.normalWorld = getUniformLocation("maps.normalWorld");
	mapLocation.diffuseColor = getUniformLocation("maps.diffuseColor");
	mapLocation.material = getUniformLocation("maps.material");

	lightLocation.positionWorld = getUniformLocation("light.position");
	lightLocation.diffuseColor = getUniformLocation("light.diffuseColor");
	lightLocation.attenuation = getUniformLocation("light.attenuation");

	eyePositionLocation = getUniformLocation("eyePosition");
	screenSizeLocation = getUniformLocation("screenSize");
	worldViewProjectionLocation = getUniformLocation("worldViewProjection");

	if (mapLocation.positionWorld == invalidLocation ||
		mapLocation.normalWorld == invalidLocation ||
		mapLocation.diffuseColor == invalidLocation ||
		mapLocation.material == invalidLocation ||

		lightLocation.positionWorld == invalidLocation ||
		lightLocation.diffuseColor == invalidLocation ||
		lightLocation.attenuation == invalidLocation ||

		eyePositionLocation == invalidLocation ||
		screenSizeLocation == invalidLocation ||
		worldViewProjectionLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::LightTechnique::setEyePosition(GLfloat x, GLfloat y, GLfloat z) const
{
	glUniform3f(eyePositionLocation, x, y, z);
}

void cgvkp::rendering::LightTechnique::setLight(PointLight const& light) const
{
	glUniform3f(lightLocation.positionWorld, light.position.x, light.position.y, light.position.z);
	glUniform3f(lightLocation.diffuseColor, light.color.r * light.diffuseIntensity, light.color.g * light.diffuseIntensity, light.color.b * light.diffuseIntensity);
	glUniform3f(lightLocation.attenuation, light.constantAttenuation, light.linearAttenuation, light.exponentialAttenuation);
}

void cgvkp::rendering::LightTechnique::setMaps() const
{
	glUniform1i(mapLocation.positionWorld, GeometryBuffer::positionView);
	glUniform1i(mapLocation.normalWorld, GeometryBuffer::normalView);
	glUniform1i(mapLocation.diffuseColor, GeometryBuffer::diffuseColor);
	glUniform1i(mapLocation.material, GeometryBuffer::material);
}

void cgvkp::rendering::LightTechnique::setScreenSize(GLsizei width, GLsizei height) const
{
	glUniform2i(screenSizeLocation, width, height);
}

void cgvkp::rendering::LightTechnique::setWorldViewProjection(glm::mat4x4 const& worldViewProjection) const
{
	glUniformMatrix4fv(worldViewProjectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}
