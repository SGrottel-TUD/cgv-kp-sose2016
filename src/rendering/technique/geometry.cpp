#include "geometry.hpp"
#include "rendering/model/graphic_model_base.hpp"

bool cgvkp::rendering::GeometryTechnique::init()
{
	if (!Technique::init())
	{
		return false;
	}
	if (!addShader(GL_VERTEX_SHADER, "shaders/geometry.vs"))
	{
		return false;
	}
	if (!addShader(GL_FRAGMENT_SHADER, "shaders/geometry.fs"))
	{
		return false;
	}
	if (!link())
	{
		return false;
	}

	materialLocation = getUniformLocation("material");
	worldViewLocation = getUniformLocation("worldView");
	worldViewProjectionLocation = getUniformLocation("worldViewProjection");

	if (materialLocation == invalidLocation ||
		worldViewLocation == invalidLocation ||
		worldViewProjectionLocation == invalidLocation)
	{
		return false;
	}

	return true;
}

void cgvkp::rendering::GeometryTechnique::setMaterial() const
{
	float specularPower = 1;
	float specularIntensity = 0;
	glUniform2f(materialLocation, specularPower, specularIntensity);
}

void cgvkp::rendering::GeometryTechnique::setWorldView(glm::mat4 const& worldView) const
{
	glUniformMatrix4fv(worldViewLocation, 1, GL_FALSE, &worldView[0][0]);
}

void cgvkp::rendering::GeometryTechnique::setWorldViewProjection(glm::mat4 const& worldViewProjection) const
{
	glUniformMatrix4fv(worldViewProjectionLocation, 1, GL_FALSE, &worldViewProjection[0][0]);
}

bool cgvkp::rendering::GeometryTechnique::renderView(view::view_base::ptr const view, glm::mat4 const& viewMatrix, glm::mat4 const& projection) const
{
	if (!view->is_valid() || !view->has_model()) return false;
	auto graphic_model = std::dynamic_pointer_cast<model::graphic_model_base>(view->get_model());
	if (graphic_model == nullptr) return false;
	setWorldView(viewMatrix * graphic_model->model_matrix);
	setWorldViewProjection(projection * viewMatrix * graphic_model->model_matrix);
	view->render();
	return true;
}
