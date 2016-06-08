#pragma once

#include <GL/glew.h>

namespace cgvkp
{
	namespace rendering
	{
		class Mesh
		{
		public:
			Mesh();
			~Mesh();
			bool init(char const* filename, bool withAdjacencies = false);
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
		};
	}
}