#pragma once

#include <list>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <memory>
#include <vector>
#include "util/texture.hpp"

namespace cgvkp
{
	namespace rendering
	{
		class Mesh
		{
		public:
			enum Flags
			{
				none			= 0x00,
				instanced		= 0x01,
				withAdjacencies	= 0x02
			};

			Mesh();
			Mesh(char const* pMeshname, Flags flags = none);
			inline ~Mesh() { deinit(); }
			bool init();
			void deinit();
			void render() const;
			void updateInstances(std::vector<glm::mat4> const& worldView);

		private:
			enum VertexAttributeLocation
			{
				position = 0,
				normal,
				textureCoord,
				worldViewMatrix,
				numAttributes
			};

			char const* pMeshname;
			Flags flags;
			GLsizei numInstances;

			GLuint vertexArray;
			GLuint vertexBuffers[numAttributes];
			GLuint indexBuffer;
			GLenum indicesMode;
			GLsizei indicesCount;
			GLenum indicesType;
			std::list<std::shared_ptr<util::texture>> textures;
		};

		inline Mesh::Flags operator|(Mesh::Flags const& lhs, Mesh::Flags const& rhs) { return static_cast<Mesh::Flags>(static_cast<int>(lhs) | static_cast<int>(rhs)); }
	}
}