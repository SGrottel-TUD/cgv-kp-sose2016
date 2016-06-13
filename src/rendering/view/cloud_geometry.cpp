#include "rendering/view/cloud_geometry.hpp"
#include "util/resource_file.hpp"
#include "util/objImporter.hpp"
#include "rendering/technique.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace cgvkp {
	namespace rendering {
		namespace view {
			cloud_geometry::cloud_geometry()
				: vao(0), indexBuffer(0), indicesMode(GL_NONE), indicesCount(0), indicesType(GL_NONE)
			{
				for (int i = 0; i < numAttributes; ++i)
				{
					vertexBuffers[i] = 0;
				}
			}
			cloud_geometry::~cloud_geometry() {
				glBindVertexArray(0);
				glDeleteVertexArrays(1, &vao);
				glDeleteBuffers(numAttributes, vertexBuffers);
				glDeleteBuffers(1, &indexBuffer);
			}
			bool cloud_geometry::init() {
				util::ObjImporter mesh("meshes/sphere.obj", true);
				if (!mesh.isValid())
				{
					return false;
				}

				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				glGenBuffers(1, &vertexBuffers[position]);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[position]);
				glBufferData(GL_ARRAY_BUFFER, mesh.getPositionsSize(), mesh.getPositions(), GL_STATIC_DRAW);
				glEnableVertexAttribArray(position);
				glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

				if (mesh.hasNormals())
				{
					glGenBuffers(1, &vertexBuffers[normal]);
					glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[normal]);
					glBufferData(GL_ARRAY_BUFFER, mesh.getNormalsSize(), mesh.getNormals(), GL_STATIC_DRAW);
					glEnableVertexAttribArray(normal);
					glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
				}
				if (mesh.hasTextureCoords())
				{
					glGenBuffers(1, &vertexBuffers[textureCoord]);
					glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[textureCoord]);
					glBufferData(GL_ARRAY_BUFFER, mesh.getTextureCoordsSize(), mesh.getTextureCoords(), GL_STATIC_DRAW);
					glEnableVertexAttribArray(textureCoord);
					glVertexAttribPointer(textureCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
				}

				glGenBuffers(1, &indexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getIndicesSize(), mesh.getIndices(), GL_STATIC_DRAW);

				glBindVertexArray(0);

				indicesMode = mesh.getIndicesMode();
				indicesCount = mesh.getIndicesCount();
				indicesType = mesh.getIndicesType();
				return true;
			}
		}
	}
}