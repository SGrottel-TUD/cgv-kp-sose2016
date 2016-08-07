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
			Mesh();
			Mesh(char const* pMeshname, bool instanced = false, bool withAdjacencies = false);
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
			bool instanced;
			GLsizei numInstances;
			bool withAdjacencies;

			GLuint vertexArray;
			GLuint vertexBuffers[numAttributes];
			GLuint indexBuffer;
			GLenum indicesMode;
			GLsizei indicesCount;
			GLenum indicesType;
			std::list<std::shared_ptr<util::texture>> textures;
		};
	}
}