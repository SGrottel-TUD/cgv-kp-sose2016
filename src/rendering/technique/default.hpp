#pragma once

#include <glm/mat4x4.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class DefaultTechnique : public Technique
		{
		public:
			inline DefaultTechnique() : worldViewProjectionLocation(invalidLocation) {}
			bool init();
			void setWorldViewProjection(glm::mat4 const& worldViewProjection) const;

		private:
			GLint worldViewProjectionLocation;
		};
	}
}
