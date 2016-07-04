#pragma once

#include <GL/glew.h>
#include <memory>
#include "util/texture.hpp"

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
			GLuint indexBuffer;
			GLenum indicesMode;
			GLsizei indicesCount;
			GLenum indicesType;
			std::shared_ptr<util::texture> texture;
		};
	}
}