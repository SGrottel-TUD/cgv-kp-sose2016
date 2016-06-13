#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include "technique.hpp"

namespace cgvkp
{
	namespace rendering
	{
		// forward declaration
		struct PointLight;

		class GeometryBuffer
		{
		public:
			enum TextureType
			{
				position = 0,
				normal,
				diffuseColor,
				material,
				final,
				numTextures
			};

			GeometryBuffer();
			~GeometryBuffer();
			bool init(GLsizei width, GLsizei height);
			void deinit();
			void bindForGeometryPass() const;
			void bindForLightPass() const;
			void bindForFinalPass() const;
			void resize(GLsizei width, GLsizei height) const;

		private:
			GLuint framebuffer;
			GLuint textures[numTextures];
			GLuint depthStencilTexture;
		};



		/* Geometry technique */

		class GeometryTechnique : public Technique
		{
		public:
			GeometryTechnique();
			virtual bool init();
			void setAmbientLight(glm::vec3 const& color) const;
			void setMaterial() const;
			void setWorld(glm::mat4x4 const& world) const;
            bool renderView(view::view_base::ptr const view, glm::mat4x4 const& projectionViewMatrix) const;

		private:
			GLint ambientLightLocation;
			GLint materialLocation;
			GLint worldLocation;
		};

		/* Light technique */

		class LightTechnique : public Technique
		{
		public:
			LightTechnique();
			virtual bool init();
			void setEyePosition(GLfloat x, GLfloat y, GLfloat z) const;
			void setLight(PointLight const& light) const;
			void setMaps() const;
			void setScreenSize(GLsizei width, GLsizei height) const;

		private:
			struct
			{
				GLint position;
				GLint diffuseColor;
				GLint attenuation;
			} lightLocation;
			struct
			{
				GLint position;
				GLint normal;
				GLint diffuseColor;
				GLint material;
			} mapLocation;
			GLint eyePositionLocation;
			GLint screenSizeLocation;
		};
	}
}
