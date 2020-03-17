#pragma once
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <stb_image.h>
#include "referenceCounter.h"
#include "../Shader.h"

using std::vector;
using namespace glm;
using namespace std;

struct Vertex {
	glm::vec3 vertices;
	glm::vec2 texture;
	glm::vec3 normal;
	Vertex(glm::vec3 pos, glm::vec2 texCoord = glm::vec2(0, 0), glm::vec3 normal = glm::vec3(0, 0, 0)):
		vertices(pos),
		texture(texCoord),
		normal(normal){}
};

class Material {

public:

	inline Material() {}
	inline ~Material() {}
	   
	unsigned int indexAlbedo = INT_MAX;
	string name;
	float shininess; // Ns on mtl
	float alpha; // d on mtl
	glm::vec3 ambient; // ka on mtl
	glm::vec3 diffuse; // kd on mtl
	glm::vec3 specular; // ks on mtl
	int ilum; // 1 -> without specular lighting, 2-> with specular lightning
};

class MeshData : public ReferenceCounter {
public:
	MeshData(int indexSize);
	virtual ~MeshData();
	inline unsigned int getVBO() { return m_vbo; }
	inline unsigned int getIBO() { return m_ibo; }
	inline unsigned int getVAO() { return m_vao; }
	inline int getSize() { return m_size; }

	Material *material;

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
	Mesh(const string& _path, const string& _mtlPath);
	Mesh(Vertex* vertices, int vSize, int* indices, int indexSize, int indexMesh, bool calcNormals);
	~Mesh();
	void Draw(Shader *shader);
	std::string name;
	glm::vec3 min;
	glm::vec3 max;
	string path;
	string mtlPath;

	vector<Material*> materials;

private:
	static std::map<std::string, MeshData*> s_resourceMap;
	// Create a Mesh with a given mesh
	Mesh(Mesh& mesh) {}
	void operator=(Mesh& mesh) {}
	void calcNormals(Vertex* vertices, int vertSize, int* indices, int indexSize);
	void initMesh(Vertex* vertices, int vertSize, int* indices, int indexSize, int indexMesh, bool calcNormals=true);
	bool loadMaterials(string path);
	unsigned int loadTexture(const char* path);
	string m_filename;
	vector<MeshData*> m_meshdata;
	bool hasMtl;
};