#pragma once

#include <glm/mat4x4.hpp>
#include "technique.hpp"
#include "rendering/lights.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class LightTechnique : public Technique
		{
		public:
			LightTechnique();
			virtual bool init();
			void setEyePosition(GLfloat x, GLfloat y, GLfloat z) const;
			void setLight(PointLight const& light) const;
			void setMaps() const;
			void setScreenSize(GLsizei width, GLsizei height) const;
			void setWorldViewProjection(glm::mat4 const& worldViewProjection) const;

		private:
			struct
			{
				GLint positionWorld;
				GLint diffuseColor;
				GLint attenuation;
			} lightLocation;
			struct
			{
				GLint positionWorld;
				GLint normalWorld;
				GLint diffuseColor;
				GLint material;
			} mapLocation;
			GLint eyePositionLocation;
			GLint screenSizeLocation;
			GLint worldViewProjectionLocation;
		};
	}
}