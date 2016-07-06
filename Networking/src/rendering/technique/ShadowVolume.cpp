#include<memory>

#include "ShadowVolume.hpp"
#include "rendering/model/graphic_model_base.hpp"

/*
Stencil pass technique
*/

cgvkp::rendering::ShadowVolumeTechnique::ShadowVolumeTechnique()
    : lightPositionLocation(invalidLocation), viewProjectionLocation(invalidLocation), worldLocation(invalidLocation)
{
}

bool cgvkp::rendering::ShadowVolumeTechnique::init()
{
    if (!Technique::init())
    {
        return false;
    }
    if (!addShader(GL_VERTEX_SHADER, "shaders/shadowVolume.vs"))
    {
        return false;
    }
    if (!addShader(GL_GEOMETRY_SHADER, "shaders/shadowVolume.gs"))
    {
        return false;
    }
    if (!addShader(GL_FRAGMENT_SHADER, "shaders/shadowVolume.fs"))
    {
        return false;
    }
    if (!link())
    {
        return false;
    }

    lightPositionLocation = getUniformLocation("lightPosition");
    viewProjectionLocation = getUniformLocation("viewProjection");
    worldLocation = getUniformLocation("world");

    if (lightPositionLocation == invalidLocation ||
        viewProjectionLocation == invalidLocation ||
        worldLocation == invalidLocation)
    {
        return false;
    }

    return true;
}

void cgvkp::rendering::ShadowVolumeTechnique::setLightPosition(glm::vec3 const& lightPosition) const
{
    glUniform3f(lightPositionLocation, lightPosition.x, lightPosition.y, lightPosition.z);
}

void cgvkp::rendering::ShadowVolumeTechnique::setViewProjection(glm::mat4x4 const& viewProjection) const
{
    glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, &viewProjection[0][0]);
}

void cgvkp::rendering::ShadowVolumeTechnique::setWorld(glm::mat4x4 const& world) const
{
    glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &world[0][0]);
}
bool cgvkp::rendering::ShadowVolumeTechnique::renderView(view::view_base::ptr const view) const {
    if (!view->is_valid() || !view->has_model() || !view->casts_shadows()) return false;
    auto graphic_model = std::dynamic_pointer_cast<model::graphic_model_base>(view->get_model());
    if (graphic_model == nullptr) return false;
    setWorld(graphic_model->model_matrix);
    view->render();
    return true;
}
