#pragma once

#include <glm/mat4x4.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class SpriteGeometryTechnique : public Technique
		{
		public:
			inline SpriteGeometryTechnique() : projectionLocation(invalidLocation) {}
			virtual bool init();
			void setProjection(glm::mat4 const& projection) const;

		private:
			GLint projectionLocation;
		};
	}
}
