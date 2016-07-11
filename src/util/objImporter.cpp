#include "objImporter.hpp"
#include "util/resource_file.hpp"
#include <fstream>
#if defined(_DEBUG) || defined(DEBUG)
#include <iostream>
#endif
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>

struct Vertex
{
	int position;
	int normal;
	int textureCoord;

	inline bool operator==(Vertex const& v) const { return position == v.position && normal == v.normal && textureCoord == v.textureCoord; }
};

struct Edge
{
	int v1;
	int v2;

	inline Edge(int a, int b) { if (a > b) { v1 = a; v2 = b; } else { v1 = b; v2 = a; } }
	inline bool operator==(Edge const& e) const { return v1 == e.v1 && v2 == e.v2; }
};

namespace std
{
	template<> struct hash<Vertex>
	{
		std::size_t operator()(const Vertex& k) const { return ((hash<int>()(k.position) ^ (hash<int>()(k.normal) << 1)) >> 1) ^ (hash<int>()(k.textureCoord) << 1); }
	};
	template<> struct hash<Edge>
	{
		std::size_t operator()(const Edge& k) const { return hash<int>()(k.v2 << 16) + k.v1; }	// could be better. Generates 16502 comparisions which will return false in case of the hand model.
	};
}

struct Face
{
	unsigned int vertices[3];
};

cgvkp::util::ObjImporter::ObjImporter(char const* pMeshname, bool withAdjacencies /* = false */)
	: verticesCount(0), positions(nullptr), normals(nullptr), textureCoords(nullptr), indices(nullptr), indicesMode(GL_NONE), indicesCount(0), indicesType(GL_NONE), indicesSize(0)
{
	load(pMeshname, withAdjacencies);
}

cgvkp::util::ObjImporter::~ObjImporter()
{
	delete[] positions;
	delete[] normals;
	delete[] textureCoords;

	switch (indicesType)
	{
	case GL_UNSIGNED_BYTE:
		delete[] static_cast<char*>(indices);
		break;

	case GL_UNSIGNED_SHORT:
		delete[] static_cast<short*>(indices);
		break;

	case GL_UNSIGNED_INT:
		delete[] static_cast<int*>(indices);
		break;
	}
}

bool cgvkp::util::ObjImporter::load(char const* pMeshname, bool withAdjacencies)
{
	std::ifstream file(resource_file::getResourcePath("meshes", std::string(pMeshname) + ".obj"));

	if (!file.good())
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not open mesh file \"" << pMeshname << ".obj\"." << std::endl;
#endif
		return false;
	}

	// Initialize the aabb with values which will be definetely overwritten.
	aabb.min.x = aabb.min.y = aabb.min.z = std::numeric_limits<float>::max();
	aabb.max.x = aabb.max.y = aabb.max.z = std::numeric_limits<float>::min();

	std::vector<glm::vec3> objPositions;
	std::vector<glm::vec3> objNormals;
	std::vector<glm::vec2> objTextureCoords;
	std::vector<Vertex const*> objVertices;
	std::unordered_map<Vertex, unsigned int> vertexToIndex;
	std::unordered_map<Edge, std::vector<unsigned int>> edgeToPoints;
	std::vector<Face> faces;
	std::vector<unsigned int> objIndices;

	bool containsNormals = false;
	bool containsTextureCoords = false;


	// Read the data.
	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream ss;
		ss << line;
		std::string type;
		ss >> type;
		if (type == "f")	// Check for faces first because it will most likely be the most defined type.
		{
			Face f;
			for (int i = 0; i < 3; ++i)
			{
				Vertex vertex;
				ss >> vertex.position;
				if (vertex.position < 0)	// Negative numbers refer relatively to the end of the list, -1 referring to the latest defined element
				{
					vertex.position += static_cast<int>(objPositions.size());
				}
				else
				{
					--vertex.position;
				}

				if (ss.get() == '/')
				{
					if (ss.peek() != '/')
					{
						ss >> vertex.textureCoord;
						if (vertex.textureCoord < 0)	// Negative numbers refer relatively to the end of the list, -1 referring to the latest defined element
						{
							vertex.textureCoord += static_cast<int>(objTextureCoords.size());
						}
						else
						{
							--vertex.textureCoord;
						}
						containsTextureCoords = true;
					}
					else
					{
						vertex.textureCoord = -1;
					}

					if (ss.get() == '/')
					{
						ss >> vertex.normal;
						if (vertex.normal < 0)	// Negative numbers refer relatively to the end of the list, -1 referring to the latest defined element
						{
							vertex.normal += static_cast<int>(objNormals.size());
						}
						else
						{
							--vertex.normal;
						}
						containsNormals = true;
					}
					else
					{
						vertex.normal = -1;
					}
				}
				else
				{
					vertex.textureCoord = -1;
					vertex.normal = -1;
				}

				auto duplicate = vertexToIndex.find(vertex);
				if (duplicate != vertexToIndex.end())
				{
					f.vertices[i] = duplicate->second;
				}
				else
				{
					auto pair = vertexToIndex.insert({ vertex, static_cast<unsigned int>(objVertices.size()) });
					objVertices.push_back(&pair.first->first);
					f.vertices[i] = pair.first->second;
				}
			}
			faces.push_back(f);

			// Save the adjacent Vertex for each edge (remember only positional index as edges)
			if (withAdjacencies)
			{
				edgeToPoints[Edge(objVertices[f.vertices[0]]->position, objVertices[f.vertices[1]]->position)].push_back(f.vertices[2]);
				edgeToPoints[Edge(objVertices[f.vertices[0]]->position, objVertices[f.vertices[2]]->position)].push_back(f.vertices[1]);
				edgeToPoints[Edge(objVertices[f.vertices[1]]->position, objVertices[f.vertices[2]]->position)].push_back(f.vertices[0]);
			}
		}
		else if (type == "v")
		{
			glm::vec3 v;
			ss >> v.x;
			ss >> v.y;
			ss >> v.z;
			objPositions.push_back(v);

			if (v.x < aabb.min.x) { aabb.min.x = v.x; }
			if (v.y < aabb.min.y) { aabb.min.y = v.y; }
			if (v.z < aabb.min.z) { aabb.min.z = v.z; }
			if (v.x > aabb.max.x) { aabb.max.x = v.x; }
			if (v.y > aabb.max.y) { aabb.max.y = v.y; }
			if (v.z > aabb.max.z) { aabb.max.z = v.z; }
		}
		else if (type == "vt")
		{
			glm::vec2 vt;
			ss >> vt.x;
			ss >> vt.y;
			objTextureCoords.push_back(vt);
		}
		else if (type == "vn")
		{
			glm::vec3 vn;
			ss >> vn.x;
			ss >> vn.y;
			ss >> vn.z;
			objNormals.push_back(vn);
		}
		else if (type == "mtllib")	// Materials not (fully) supported.
		{
			// Do a quick scan for the texture file name in the mtl file
			std::string mtlFile;
			ss >> mtlFile;
			// Adapt directory

			readTexturePathFromMtl("meshes/" + mtlFile);
		}
		else if (type == "usemtl")
		{
		}
		else if (type == "s")	// Smooth shading not supported.
		{
		}
		else if (type == "o")	// Object name not supported.
		{
		}
		else if (type == "g")	// Group names not supported.
		{
		}
		else if (type[0] == '#')	// Comment. Maybe it contains useful information like number of vertices.
		{
			std::string infoType;
			ss >> infoType;
			if (infoType == "Vertices:")
			{
				int num;
				ss >> num;
				objPositions.reserve(num);
				objNormals.reserve(num);
				objTextureCoords.reserve(num);
				objVertices.reserve(num);
				vertexToIndex.reserve(num);
			}
			if (infoType == "Faces:")
			{
				int num;
				ss >> num;
				faces.reserve(num);
				edgeToPoints.reserve((3 * num) / 2);	// 2 * number of edges = 3 * number of faces
			}
		}
		else if (type == "")	// Empty line.
		{
		}
		else
		{
#if defined(_DEBUG) || defined(DEBUG)
			std::cerr << pMeshname << ".obj: Unknown type \"" << type << "\"." << std::endl;
#endif
			return false;
		}
	}
	file.close();


	try {
		// Create data for the vertex buffers.
		verticesCount = static_cast<GLsizei>(objVertices.size());
		positions = new glm::vec3[verticesCount];
		for (GLsizei i = 0; i < verticesCount; ++i)
		{
			positions[i] = objPositions[objVertices[i]->position];
		}
		objPositions.clear();		// Not needed anymore.
		if (containsNormals)
		{
			normals = new glm::vec3[verticesCount];
			for (GLsizei i = 0; i < verticesCount; ++i)
			{
				normals[i] = objNormals[objVertices[i]->normal];
			}
		}
		objNormals.clear();			// Not needed anymore.
		if (containsTextureCoords)
		{
			textureCoords = new glm::vec2[verticesCount];
			for (GLsizei i = 0; i < verticesCount; ++i)
			{
				textureCoords[i] = objTextureCoords[objVertices[i]->textureCoord];
			}
		}
		objTextureCoords.clear();	// Not needed anymore.


									// Create data for the index buffers.
		objIndices.reserve(faces.size() * (withAdjacencies ? 6 : 3));
		for (auto const& f : faces)
		{
			objIndices.push_back(f.vertices[0]);
			if (withAdjacencies)
			{
				std::vector<unsigned int> const& edge = edgeToPoints[Edge(objVertices[f.vertices[0]]->position, objVertices[f.vertices[1]]->position)];
				if (edge.size() != 2)
				{
#if defined(_DEBUG) || defined(DEBUG)
					if (edge.size() == 1)
					{
						std::cerr << "Could not create adjacencies. The mesh " << pMeshname << ".obj has a hole." << std::endl;
					}
					else
					{
						std::cerr << "Could not create adjacencies. The mesh " << pMeshname << ".obj has an edge which is used by more than two triangles." << std::endl;
					}
#endif
					return false;
				}
				if (edge[0] != f.vertices[2])
				{
					objIndices.push_back(edge[0]);
				}
				else
				{
					objIndices.push_back(edge[1]);
				}
			}
			objIndices.push_back(f.vertices[1]);
			if (withAdjacencies)
			{
				std::vector<unsigned int> const& edge = edgeToPoints[Edge(objVertices[f.vertices[1]]->position, objVertices[f.vertices[2]]->position)];
				if (edge.size() != 2)
				{
#if defined(_DEBUG) || defined(DEBUG)
					if (edge.size() == 1)
					{
						std::cerr << "Could not create adjacencies. The mesh " << pMeshname << ".obj has a hole." << std::endl;
					}
					else
					{
						std::cerr << "Could not create adjacencies. The mesh " << pMeshname << ".obj has an edge which is used by more than two triangles." << std::endl;
					}
#endif
					return false;
				}
				if (edge[0] != f.vertices[0])
				{
					objIndices.push_back(edge[0]);
				}
				else
				{
					objIndices.push_back(edge[1]);
				}
			}
			objIndices.push_back(f.vertices[2]);
			if (withAdjacencies)
			{
				std::vector<unsigned int> const& edge = edgeToPoints[Edge(objVertices[f.vertices[2]]->position, objVertices[f.vertices[0]]->position)];
				if (edge.size() != 2)
				{
#if defined(_DEBUG) || defined(DEBUG)
					if (edge.size() == 1)
					{
						std::cerr << "Could not create adjacencies. The mesh " << pMeshname << ".obj has a hole." << std::endl;
					}
					else
					{
						std::cerr << "Could not create adjacencies. The mesh " << pMeshname << ".obj has an edge which is used by more than two triangles." << std::endl;
					}
#endif
					return false;
				}
				if (edge[0] != f.vertices[1])
				{
					objIndices.push_back(edge[0]);
				}
				else
				{
					objIndices.push_back(edge[1]);
				}
			}
		}
	}
#if defined(_DEBUG) || defined(DEBUG)
	catch (std::exception const& e)	// Could be an index out of bounds.
	{
		std::cerr << "Exception during parsing \"" << pMeshname << ".obj\": " << e.what() << std::endl;
#else
	catch (std::exception const&)	// Could be an index out of bounds.
	{
#endif
		return false;
	}

	indicesMode = (withAdjacencies ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES);
	indicesCount = static_cast<GLsizei>(objIndices.size());


	// Shrink buffer size.
	if (objVertices.size() < 0x100)	// Byte
	{
		indicesType = GL_UNSIGNED_BYTE;
		unsigned char* charIndices = new unsigned char[indicesCount];
		for (GLsizei i = 0; i < indicesCount; ++i)
		{
			charIndices[i] = objIndices[i];
		}
		indices = charIndices;
		indicesSize = indicesCount * sizeof(unsigned char);
	}
	else if (objVertices.size() < 0x10000)	// Short
	{
		indicesType = GL_UNSIGNED_SHORT;
		unsigned short* shortIndices = new unsigned short[indicesCount];
		for (GLsizei i = 0; i < indicesCount; ++i)
		{
			shortIndices[i] = objIndices[i];
		}
		indices = shortIndices;
		indicesSize = indicesCount * sizeof(unsigned short);
	}
	else	// Integer
	{
		indicesType = GL_UNSIGNED_INT;
		unsigned int* intIndices = new unsigned int[indicesCount];
		for (GLsizei i = 0; i < indicesCount; ++i)
		{
			intIndices[i] = objIndices[i];
		}
		indices = intIndices;
		indicesSize = indicesCount * sizeof(unsigned int);
	}

	return true;
	}

bool cgvkp::util::ObjImporter::readTexturePathFromMtl(std::string const& filename)
{
	std::ifstream file(resource_file::find_resource_file(filename));

	if (!file.good())
	{
#if defined(_DEBUG) || defined(DEBUG)
		std::cerr << "Could not open mtl file \"" << filename << "\"." << std::endl;
#endif
		return false;
	}

	// Read the data.
	std::string line;
	bool pathRead = false;
	while (!pathRead && std::getline(file, line))
	{
		std::stringstream ss;
		ss << line;
		std::string type;
		ss >> type;

		if (type == "map_Kd")
		{
			ss >> texturePath;
#if defined(_DEBUG) || defined(DEBUG)
			std::cout << "Got texture path: " << texturePath << std::endl;
#endif
			pathRead = true;
		}
	}
	file.close();

	return pathRead;
}
