#pragma once

#include <gl/glew.h>
#include <glm/common.hpp>

namespace cgvkp
{
	namespace util
	{
		class ObjImporter
		{
		public:
			ObjImporter(char const* filename, bool withAdjacencies = false);
			~ObjImporter();
			inline glm::vec3 const* getPositions() { return positions; }
			inline glm::vec3 const* getNormals() { return normals; }
			inline glm::vec2 const* getTextureCoords() { return textureCoords; }
			inline void const* getIndices() { return indices; }
			inline GLsizei getVerticesCount() { return verticesCount; }
			inline GLenum getIndicesMode() { return indicesMode; }
			inline GLsizei getIndicesCount() { return indicesCount; }
			inline GLenum getIndicesType() { return indicesType; }
			inline GLsizeiptr getPositionsSize() { return verticesCount * sizeof(glm::vec3); }
			inline GLsizeiptr getNormalsSize() { return verticesCount * sizeof(glm::vec3); }
			inline GLsizeiptr getTextureCoordsSize() { return verticesCount * sizeof(glm::vec2); }
			inline GLsizeiptr getIndicesSize() { return indicesSize; }
			inline bool hasNormals() { return normals != nullptr; }
			inline bool hasTextureCoords() { return textureCoords != nullptr; }
			inline bool isValid() { return indices != nullptr; }

		private:
			bool load(char const* filename, bool withAdjacencies);

			GLsizei verticesCount;
			glm::vec3* positions;
			glm::vec3* normals;
			glm::vec2* textureCoords;
			void* indices;
			GLenum indicesMode;
			GLsizei indicesCount;
			GLenum indicesType;
			GLsizeiptr indicesSize;
		};
	}
}
