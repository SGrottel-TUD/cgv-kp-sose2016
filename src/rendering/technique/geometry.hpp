#pragma once

#include <glm/mat4x4.hpp>
#include "technique.hpp"
#include "rendering/view/view_base.hpp"

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
			bool renderView(view::view_base::ptr const view, glm::mat4 const& viewMatrix, glm::mat4 const& projection) const;

		private:
			GLint materialLocation;
			GLint worldViewLocation;
			GLint worldViewProjectionLocation;
		};
	}
}
