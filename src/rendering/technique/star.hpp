#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class StarTechnique : public Technique
		{
		public:
			inline StarTechnique() : textureCoordOffsetLocation(invalidLocation), worldViewProjectionLocation(invalidLocation) {}
			bool init();
			void setTextureCoordOffset(glm::vec2 const& offset) const;
			void setWorldViewProjection(glm::mat4 const& worldViewProjection) const;

		private:
			GLint textureCoordOffsetLocation;
			GLint worldViewProjectionLocation;
		};
	}
}
