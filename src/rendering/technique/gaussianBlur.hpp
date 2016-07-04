#pragma once

#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class GaussianBlurTechnique : public Technique
		{
		public:
			enum Direction
			{
				horizontal,
				vertical
			};

			inline GaussianBlurTechnique() : blurSamplerLocation(invalidLocation), blurSizeLocation(invalidLocation), directionLocation(invalidLocation) {}
			bool init();
			void setBlurSize(GLsizei size) const;
			void setDirection(Direction direction) const;

		private:
			GLint blurSamplerLocation;
			GLint blurSizeLocation;
			GLint directionLocation;
		};
	}
}
