#pragma once

#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		struct DirectionalLight;

		class DirectionalLightTechnique : public Technique
		{
		public:
			inline DirectionalLightTechnique() : diffuseColorLocation(invalidLocation), directionLocation(invalidLocation) {}
			bool init();
			void setLight(DirectionalLight const& light) const;

		private:
			GLint directionLocation;
			GLint diffuseColorLocation;
		};
	}
}
