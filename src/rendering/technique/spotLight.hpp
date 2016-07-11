#pragma once

#include <glm/mat4x4.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		struct SpotLight;

		class SpotLightTechnique : public Technique
		{
		public:
			SpotLightTechnique() : projectionLocation(invalidLocation), lightLocation{ invalidLocation, invalidLocation, invalidLocation, invalidLocation, invalidLocation } {}
			bool init();
			void setLight(SpotLight const& light, glm::mat4 const& view, glm::mat4 const& projection) const;

		private:
			GLint projectionLocation;
			struct
			{
				GLint color;
				GLint position;
				GLint direction;
				GLint attenuation;
				GLint cutoff;
			} lightLocation;
		};
	}
}
