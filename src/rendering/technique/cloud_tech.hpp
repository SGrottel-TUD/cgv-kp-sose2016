#pragma once

#include <glm/mat4x4.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class CloudTechnique : public Technique
		{
		public:
			inline CloudTechnique() : worldViewProjectionLocation(invalidLocation) {}
			bool init();
			void setWorldViewProjection(glm::mat4 const& m) const;

		private:
			GLint worldViewProjectionLocation;
		};
	}
}
