#pragma once

#include <glm/vec3.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class SSAOTechnique : public Technique
		{
		public:
			inline SSAOTechnique() : ambientLightLocation(invalidLocation) {}
			bool init();
			void setAmbientLight(glm::vec3 const& color) const;

		private:
			GLint ambientLightLocation;
		};
	}
}
