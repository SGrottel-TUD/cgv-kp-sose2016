#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace cgvkp
{
	namespace rendering
	{
		struct Light
		{
		public:
			glm::vec3 color;

			float diffuseIntensity;

		protected:
			inline Light() : diffuseIntensity(0) {}
		};



		/* Directional light */

		struct DirectionalLight : public Light
		{
		public:
			glm::vec3 direction;
		};



		/* Point light */

		struct PointLight : public Light
		{
		public:
			inline PointLight() : attenuation(1, 0, 0) {}
			float calculateMaxDistance() const;

			glm::vec3 position;
			glm::vec3 attenuation;	// vec3(constant, linear, exponential)
		};



		/* Spot light */

		struct SpotLight : public PointLight
		{
		public:
			SpotLight() : cutoff(0) {}

			float cutoff;
			glm::vec3 direction;
		};
	}
}