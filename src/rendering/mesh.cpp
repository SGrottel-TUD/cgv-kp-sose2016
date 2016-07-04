#include "mesh.hpp"
#include "util/objImporter.hpp"

cgvkp::rendering::Mesh::Mesh()
	: vertexArray(0), indexBuffer(0), indicesMode(GL_NONE), indicesCount(0), indicesType(GL_NONE)
{
	for (int i = 0; i < numAttributes; ++i)
	{
		vertexBuffers[i] = 0;
	}
}

bool cgvkp::rendering::Mesh::init(char const* pMeshname, bool withAdjacencies /* = false */)
{
	util::ObjImporter mesh(pMeshname, withAdjacencies);
	if (!mesh.isValid())
	{
		return false;
	}

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

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
		if (!mesh.getTexturePath().empty())
		{
			texture = util::texture::from_png(mesh.getTexturePath());
		}
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

void cgvkp::rendering::Mesh::deinit()
{
	if (vertexArray)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &vertexArray);
		vertexArray = 0;
	}
	if (vertexBuffers[0])
	{
		glDeleteBuffers(numAttributes, vertexBuffers);
		for (int i = 0; i < numAttributes; ++i)
		{
			vertexBuffers[i] = 0;
		}
	}
	if (indexBuffer)
	{
		glDeleteBuffers(1, &indexBuffer);
		indexBuffer = 0;
	}
	texture.reset();
}

void cgvkp::rendering::Mesh::render() const
{
	glBindVertexArray(vertexArray);
	if (texture)
	{
		texture->bind_texture();
	}
	glDrawElements(indicesMode, indicesCount, indicesType, nullptr);
}
