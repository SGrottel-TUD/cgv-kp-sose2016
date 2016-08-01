#include "cloudController.hpp"
#include "data/world.hpp"
#include "rendering/release_renderer.hpp"
#include "rendering/model/cloud_model.hpp"
#include <glm/glm.hpp>
#include <random>

cgvkp::rendering::controller::CloudController::CloudController(release_renderer* pRenderer, data::world const& d, Mesh const& mesh)
{
	float const distanceToGameArea = 4;
	float const distanceMinX = 0.1f;
	float const distanceMaxX = 0.5f;
	float const distanceY = 0.1f;
	float const distanceZ = 0.03f;

	w = d.get_config().width();
	float h = d.get_config().height();

	std::default_random_engine generator;
	std::uniform_real_distribution<float> xDistribution(distanceMinX, distanceMaxX);
	std::uniform_real_distribution<float> yDistribution(-distanceY, distanceY);
	std::uniform_real_distribution<float> angleDistribution(0, glm::two_pi<float>());
	std::uniform_real_distribution<float> velocityDistribution(-0.2f, 0.2f);

	for (float z = -h; z < 0; z += distanceZ * (z < -1 ? -z : 1))
	{
		float wZ = tan(glm::quarter_pi<float>()) * (distanceToGameArea - z) - w;

		float distanceX = xDistribution(generator);

		std::uniform_real_distribution<float> scaleDistribution(0.5f * distanceX, 0.6f * distanceX);
		
		for (float x = -wZ; x < w + wZ; x += distanceX)
		{
			auto model = std::make_shared<model::cloud_model>();
			model->position = glm::vec3(x, yDistribution(generator), z);
			model->rotation = glm::quat(glm::vec3(0, 0, angleDistribution(generator)));
			model->scale = glm::vec3(scaleDistribution(generator));
			model->velocity = glm::vec3(velocityDistribution(generator), velocityDistribution(generator), 0);
			model->model_matrix = glm::translate(model->position) * glm::toMat4(model->rotation) * glm::scale(model->scale);
			pRenderer->add_model(model);
			clouds.push_back(model);

			auto view = std::make_shared<view::cloud_view>(mesh);
			view->set_model(model);
			pRenderer->add_view(view);
		}
	}
}

void cgvkp::rendering::controller::CloudController::update(double seconds, std::shared_ptr<abstract_user_input> input)
{
	float const distanceToGameArea = 4;
	float const distanceY = 0.2f;

	std::default_random_engine generator;
	std::uniform_real_distribution<float> angleDistribution(-glm::radians(2.0f), glm::radians(2.0f));
	for (auto const& pCloud : clouds)
	{
		pCloud->position += pCloud->velocity * static_cast<float>(seconds);
		float wZ = tan(glm::quarter_pi<float>()) * (distanceToGameArea - pCloud->position.z) - w;
		if (pCloud->position.x > w + wZ)
		{
			pCloud->velocity.x = -glm::abs(pCloud->velocity.x);
		}
		if (pCloud->position.x < -wZ)
		{
			pCloud->velocity.x = glm::abs(pCloud->velocity.x);
		}
		if (pCloud->position.y > distanceY)
		{
			pCloud->velocity.y = -glm::abs(pCloud->velocity.y);
		}
		if (pCloud->position.y < -distanceY)
		{
			pCloud->velocity.y = glm::abs(pCloud->velocity.y);
		}
		pCloud->rotation *= glm::quat(glm::vec3(0, 0, angleDistribution(generator)));
		pCloud->model_matrix = glm::translate(pCloud->position) * glm::toMat4(pCloud->rotation) * glm::scale(pCloud->scale);
	}
}
