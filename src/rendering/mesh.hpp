#pragma once

#include <GL/glew.h>
#include <memory>
#include "util/texture.hpp"
#include <vector>

namespace cgvkp
{
	namespace rendering
	{
		class Mesh
		{
		public:
			Mesh();
			inline ~Mesh() { deinit(); }
			bool init(char const* pMeshname, bool withAdjacencies = false);
			void deinit();
			void render() const;
			void instancedRender(std::vector<glm::mat4> model_matrices) const;

		private:
			enum VertexAttributeLocation
			{
				position = 0,
				normal,
				textureCoord,
				numAttributes
			};

			GLuint vertexArray;
			GLuint vertexBuffers[numAttributes];
			GLuint model_matrices_VBO;
			GLuint indexBuffer;
			GLenum indicesMode;
			GLsizei indicesCount;
			GLenum indicesType;
			std::shared_ptr<util::texture> texture;
		};
	}
}