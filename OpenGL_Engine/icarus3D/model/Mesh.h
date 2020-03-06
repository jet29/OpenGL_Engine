#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>
#include "referenceCounter.h"

using std::vector;
using namespace glm;
using namespace std;

struct Vertex {
	glm::vec3 vertices;
	glm::vec3 normal;
	glm::vec2 texture;
	Vertex(glm::vec3 pos, glm::vec2 texCoord = glm::vec2(0, 0), glm::vec3 normal = glm::vec3(0, 0, 0)) :
		vertices(pos),
		texture(texCoord),
		normal(normal) {}
};

class MeshData : public ReferenceCounter {
public:
	MeshData(int indexSize);
	virtual ~MeshData();
	inline unsigned int getVBO() { return m_vbo; }
	inline unsigned int getIBO() { return m_ibo; }
	inline unsigned int getVAO() { return m_vao; }
	inline int getSize() { return m_size; }
private:
	MeshData(MeshData& other) {}
	void operator=(MeshData& other) {}

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	int m_size;
};

class Mesh {
public:
	Mesh(const string& filename);
	Mesh(Vertex* vertices, int vSize, int* indices, int indexSize, int indexMesh, bool calcNormals);
	~Mesh();
	void Draw() const;
	std::string name;
	glm::vec3 min;
	glm::vec3 max;
	string path;


private:
	static std::map<std::string, MeshData*> s_resourceMap;
	// Create a Mesh with a given mesh
	Mesh(Mesh& mesh) {}
	void operator=(Mesh& mesh) {}
	void calcNormals(Vertex* vertices, int vertSize, int* indices, int indexSize);
	void initMesh(Vertex* vertices, int vertSize, int* indices, int indexSize, int indexMesh, bool calcNormals=true);

	string m_filename;
	vector<MeshData*> m_meshdata;
};