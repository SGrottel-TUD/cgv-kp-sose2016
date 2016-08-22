#pragma once

#include <glm/mat4x4.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class GeometryTechnique : public Technique
		{
		public:
			inline GeometryTechnique() : projectionLocation(invalidLocation), worldViewLocation(invalidLocation) {}
			virtual bool init();
			void setProjection(glm::mat4 const& projection) const;
			void setWorldView(glm::mat4 const& worldView) const;

		private:
			GLint projectionLocation;
			GLint worldViewLocation;
		};
	}
}
