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
			inline GeometryTechnique() : materialLocation(invalidLocation), worldViewLocation(invalidLocation), worldViewProjectionLocation(invalidLocation) {}
			virtual bool init();
			void setMaterial() const;
			void setWorldView(glm::mat4 const& worldView) const;
			void setWorldViewProjection(glm::mat4 const& worldViewProjection) const;

		private:
			GLint materialLocation;
			GLint worldViewLocation;
			GLint worldViewProjectionLocation;
		};
	}
}
