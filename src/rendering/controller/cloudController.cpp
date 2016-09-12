#include "cloudController.hpp"
#include "data/world.hpp"
#include "rendering/release_renderer.hpp"
#include "rendering/model/cloud_model.hpp"
#include <glm/glm.hpp>
#include <random>

cgvkp::rendering::controller::CloudController::CloudController(release_renderer* pRenderer, data::world const& d, Mesh& _mesh, glm::mat4 const& _view)
	: mesh(_mesh), view(_view)
{
	float const distanceToGameArea = 4;
	float const distanceMinX = 0.4f;
	float const distanceMaxX = 0.5f;
	float const distanceY = 0.1f;
	float const distanceZ = 0.03f;

	w = d.get_config().width();
	float h = d.get_config().height();

	std::default_random_engine generator;
	std::uniform_real_distribution<float> xDistribution(distanceMinX, distanceMaxX);
	std::uniform_real_distribution<float> yDistribution(-distanceY, distanceY);
	std::uniform_real_distribution<float> angleDistribution(-glm::quarter_pi<float>(), glm::quarter_pi<float>());
	std::uniform_real_distribution<float> velocityDistribution(-0.05f, 0.05f);

	for (float z = -h; z < 0; z += distanceZ * (z < -1 ? -z : 1))
	{
		float wZ = tan(glm::quarter_pi<float>()) * (distanceToGameArea - z) - w;

		float distanceX = xDistribution(generator);

		std::uniform_real_distribution<float> scaleDistribution(0.2f, 0.3f);
		
		for (float x = -wZ; x < w + wZ; x += distanceX)
		{
			auto model = std::make_shared<model::cloud_model>();
			model->position = glm::vec3(x, yDistribution(generator), z);
			model->dAngle = angleDistribution(generator);
			model->rotation = glm::quat(glm::vec3(0, 0, model->dAngle));
			model->scale = glm::vec3(scaleDistribution(generator));
			model->velocity = glm::vec3(velocityDistribution(generator), velocityDistribution(generator), 0);
			pRenderer->add_model(model);
			clouds.push_back(model);

			auto view = std::make_shared<view::cloud_view>(mesh);
			view->set_model(model);
			pRenderer->add_view(view);
		}
	}
}

void cgvkp::rendering::controller::CloudController::setCameraPosition(glm::vec3 const& position)
{
	float angleToView = acos(position.z / glm::length(position));
	rotation = glm::quat(glm::vec3(0, angleToView, 0));
}

void cgvkp::rendering::controller::CloudController::update(double seconds, std::shared_ptr<abstract_user_input> input)
{
	float const distanceToGameArea = 4;
	float const distanceY = 0.2f;

	std::vector<glm::mat4> worldViewMatrices(clouds.size());
#pragma omp parallel for
	for (int i = 0; i < clouds.size(); ++i)
	{
		clouds[i]->position += clouds[i]->velocity * static_cast<float>(seconds);
		float wZ = tan(glm::quarter_pi<float>()) * (distanceToGameArea - clouds[i]->position.z) - w;
		if (clouds[i]->position.x > w + wZ)
		{
			clouds[i]->velocity.x = -glm::abs(clouds[i]->velocity.x);
		}
		if (clouds[i]->position.x < -wZ)
		{
			clouds[i]->velocity.x = glm::abs(clouds[i]->velocity.x);
		}
		if (clouds[i]->position.y > distanceY)
		{
			clouds[i]->velocity.y = -glm::abs(clouds[i]->velocity.y);
		}
		if (clouds[i]->position.y < -distanceY)
		{
			clouds[i]->velocity.y = glm::abs(clouds[i]->velocity.y);
		}
		clouds[i]->rotation *= glm::quat(glm::vec3(0, 0, clouds[i]->dAngle * static_cast<float>(seconds)));
		worldViewMatrices[i] = view * glm::translate(clouds[i]->position) * glm::toMat4(rotation * clouds[i]->rotation) * glm::scale(clouds[i]->scale);
	}
	mesh.updateInstances(worldViewMatrices);
}
