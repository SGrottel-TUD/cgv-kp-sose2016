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
			inline GeometryTechnique() : materialLocation(invalidLocation), projectionLocation(invalidLocation), worldViewLocation(invalidLocation) {}
			virtual bool init();
			void setMaterial() const;
			void setProjection(glm::mat4 const& projection) const;
			void setWorldView(glm::mat4 const& worldView) const;

		private:
			GLint materialLocation;
			GLint projectionLocation;
			GLint worldViewLocation;
		};
	}
}
