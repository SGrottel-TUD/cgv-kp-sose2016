#pragma once

#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <vector>
#include "controller_base.hpp"

namespace cgvkp
{
	namespace data
	{
		class world;
	}

	namespace rendering
	{
		namespace model
		{
			class cloud_model;
		}
		class release_renderer;
		class Mesh;

		namespace controller
		{
			class CloudController : public controller_base
			{
			public:
				CloudController(release_renderer* pRenderer, data::world const& d, Mesh const& mesh);
				void setCameraPosition(glm::vec3 const& position);
				void update(double seconds, std::shared_ptr<abstract_user_input> input);
				inline bool has_model() const { return true; }

			private:
				float w;
				glm::quat rotation;
				std::vector<std::shared_ptr<model::cloud_model>> clouds;
			};
		}
	}
}
