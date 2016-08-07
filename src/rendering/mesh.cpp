#include "mesh.hpp"
#include "util/objImporter.hpp"

cgvkp::rendering::Mesh::Mesh()
	: pMeshname(nullptr), instanced(false), withAdjacencies(false), vertexArray(0), indexBuffer(0), indicesMode(GL_NONE), indicesCount(0), indicesType(GL_NONE)
{
	for (int i = 0; i < numAttributes; ++i)
	{
		vertexBuffers[i] = 0;
	}
}

cgvkp::rendering::Mesh::Mesh(char const* _pMeshname, bool _instanced /* = false */, bool _withAdjacencies /* = false */)
	: pMeshname(_pMeshname), instanced(_instanced), withAdjacencies(_withAdjacencies), vertexArray(0), indexBuffer(0), indicesMode(GL_NONE), indicesCount(0), indicesType(GL_NONE)
{
	for (int i = 0; i < numAttributes; ++i)
	{
		vertexBuffers[i] = 0;
	}
}

bool cgvkp::rendering::Mesh::init()
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
	if (instanced)
	{
		glVertexAttribDivisor(position, 0);
	}

	if (mesh.hasNormals())
	{
		glGenBuffers(1, &vertexBuffers[normal]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[normal]);
		glBufferData(GL_ARRAY_BUFFER, mesh.getNormalsSize(), mesh.getNormals(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(normal);
		glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		if (instanced)
		{
			glVertexAttribDivisor(normal, 0);
		}
	}
	if (mesh.hasTextureCoords())
	{
		glGenBuffers(1, &vertexBuffers[textureCoord]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[textureCoord]);
		glBufferData(GL_ARRAY_BUFFER, mesh.getTextureCoordsSize(), mesh.getTextureCoords(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(textureCoord);
		glVertexAttribPointer(textureCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		for(auto const& texturePath : mesh.getTexturePaths())
		{
			textures.push_back(util::texture::from_png(texturePath));
		}
		if (instanced)
		{
			glVertexAttribDivisor(textureCoord, 0);
		}
	}

	if (instanced)
	{
		glGenBuffers(1, &vertexBuffers[worldViewMatrix]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[worldViewMatrix]);
		for (int i = 0; i < 4; ++i)	// Matrices are 4 x 4 vectors. glVertexAttribPointer only accepts sizes <= 4
		{
			glEnableVertexAttribArray(worldViewMatrix + i);
			glVertexAttribPointer(worldViewMatrix + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<GLvoid const*>(4 * sizeof(GLfloat) * i));
			glVertexAttribDivisor(worldViewMatrix + i, 1);
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
	for (auto& pTexture : textures)
	{
		pTexture.reset();
	}
	textures.clear();
}

void cgvkp::rendering::Mesh::render() const
{
	int i = 0;
	for (auto& pTexture : textures)
	{
		glActiveTexture(GL_TEXTURE0 + i++);
		glBindTexture(GL_TEXTURE_2D, pTexture->id());
	}
	glBindVertexArray(vertexArray);
	glDrawElements(indicesMode, indicesCount, indicesType, nullptr);
}

void cgvkp::rendering::Mesh::renderInstanced(std::vector<glm::mat4> const& worldView) const
{
	GLsizei numInstances = static_cast<GLsizei>(worldView.size());
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[worldViewMatrix]);
	glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), worldView.data(), GL_STREAM_DRAW);

	int i = 0;
	for (auto& pTexture : textures)
	{
		glActiveTexture(GL_TEXTURE0 + i++);
		glBindTexture(GL_TEXTURE_2D, pTexture->id());
	}
	glBindVertexArray(vertexArray);
	glDrawElementsInstanced(indicesMode, indicesCount, indicesType, nullptr, numInstances);
}
