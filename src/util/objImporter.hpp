#pragma once

#include <gl/glew.h>
#include <glm/common.hpp>
#include <string>

namespace cgvkp
{
	namespace util
	{
		class ObjImporter
		{
		public:
			ObjImporter(std::string const& filename, bool withAdjacencies = false);
			~ObjImporter();
			inline glm::vec3 const* getPositions() const { return positions; }
			inline glm::vec3 const* getNormals() const { return normals; }
			inline glm::vec2 const* getTextureCoords() const { return textureCoords; }
			inline void const* getIndices() const { return indices; }
			inline GLsizei getVerticesCount() const { return verticesCount; }
			inline GLenum getIndicesMode() const { return indicesMode; }
			inline GLsizei getIndicesCount() const { return indicesCount; }
			inline GLenum getIndicesType() const { return indicesType; }
			inline GLsizeiptr getPositionsSize() const { return verticesCount * sizeof(glm::vec3); }
			inline GLsizeiptr getNormalsSize() const { return verticesCount * sizeof(glm::vec3); }
			inline GLsizeiptr getTextureCoordsSize() const { return verticesCount * sizeof(glm::vec2); }
			inline GLsizeiptr getIndicesSize() const { return indicesSize; }
			inline bool hasNormals() const { return normals != nullptr; }
			inline bool hasTextureCoords() const { return textureCoords != nullptr; }
			inline bool isValid() const { return indices != nullptr; }
            inline std::string getTexturePath() const { return texturePath; }

		private:
			bool load(std::string const& filename, bool withAdjacencies);
            bool readTexturePathFromMtl(std::string const& filename);

			GLsizei verticesCount;
			glm::vec3* positions;
			glm::vec3* normals;
			glm::vec2* textureCoords;
			void* indices;
			GLenum indicesMode;
			GLsizei indicesCount;
			GLenum indicesType;
			GLsizeiptr indicesSize;
            std::string texturePath;
		};
	}
}
