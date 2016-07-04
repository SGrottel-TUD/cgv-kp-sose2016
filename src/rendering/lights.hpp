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
			inline PointLight() : constantAttenuation(1), linearAttenuation(0), exponentialAttenuation(0) {}
			void calculateWorld();

			glm::vec3 position;

			float constantAttenuation;
			float linearAttenuation;
			float exponentialAttenuation;

			glm::mat4 world;
		};
	}
}
