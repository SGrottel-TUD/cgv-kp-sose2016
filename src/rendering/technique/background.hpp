#pragma once

#include <glm/glm.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class BackgroundTechnique : public Technique
		{
		public:
			inline BackgroundTechnique() : normalLocation(invalidLocation), projectionLocation(invalidLocation), worldViewLocation(invalidLocation) {}
			virtual bool init();
			void setNormal(glm::vec3 const& normal) const;
			void setProjection(glm::mat4 const& projection) const;
			void setWorldView(glm::mat4 const& worldView) const;

		private:
			GLint normalLocation;
			GLint projectionLocation;
			GLint worldViewLocation;
		};
	}
}
