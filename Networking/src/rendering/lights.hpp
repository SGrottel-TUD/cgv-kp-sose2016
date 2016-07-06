#pragma once

#include <glm/vec3.hpp>
#include <iostream>

namespace cgvkp
{
	namespace rendering
	{
		struct Light
		{
		public:
			glm::vec3 color;

			float ambientIntensity;
			float diffuseIntensity;

		protected:
			inline Light() : ambientIntensity(0), diffuseIntensity(0) {}
		};



		/* Point light */

		struct PointLight : public Light
		{
		public:
			inline PointLight() : constantAttenuation(1), linearAttenuation(0), exponentialAttenuation(0) {}
			void calculateWorld();

			glm::vec3 position;

			float constantAttenuation;
			float linearAttenuation;
			float exponentialAttenuation;

			glm::mat4x4 world;
		};
	}
}
