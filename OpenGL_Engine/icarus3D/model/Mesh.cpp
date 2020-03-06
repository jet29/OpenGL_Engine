#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


MeshData::MeshData(int indexSize) : ReferenceCounter() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	m_size = indexSize;
}

MeshData::~MeshData(){
	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);
	if (m_ibo)
		glDeleteBuffers(1, &m_ibo);
}

Mesh::Mesh(Vertex* vertices, int vertSize, int* indices, int indexSize, int indexMesh, bool calcNormals){
	m_filename = "";
	initMesh(vertices, vertSize, indices, indexSize, indexMesh, calcNormals);
}

Mesh::Mesh(const string& filename) {


	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_GenBoundingBoxes);
	if (!scene) {
		cout << "Mesh load failed: " << filename << endl;
		assert(0 != 0);
	}

	path = filename;

	float max_x = INT_MIN;
	float max_y = INT_MIN;
	float max_z = INT_MIN;
	float min_x = INT_MAX;
	float min_y = INT_MAX;
	float min_z = INT_MAX;


	int numMeshes = scene->mNumMeshes;
	m_meshdata = std::vector<MeshData*>(numMeshes, NULL);
	vector<aiMesh*> model = vector<aiMesh*>(numMeshes, NULL);

	for (int i = 0; i < numMeshes; i++) {
		model[i] = scene->mMeshes[i];

		//calculate AABB for all meshes
		if (model[i]->mAABB.mMin.x < min_x) {
			min_x = model[i]->mAABB.mMin.x;
		}
		if (model[i]->mAABB.mMin.y < min_y) {
			min_y = model[i]->mAABB.mMin.y;
		}
		if (model[i]->mAABB.mMin.z < min_z) {
			min_z = model[i]->mAABB.mMin.z;
		}
		if (model[i]->mAABB.mMax.x > max_x) {
			max_x = model[i]->mAABB.mMax.x;
		}
		if (model[i]->mAABB.mMax.y > max_y) {
			max_y = model[i]->mAABB.mMax.y;
		}
		if (model[i]->mAABB.mMax.z > max_z) {
			max_z = model[i]->mAABB.mMax.z;
		}
		// ------> end calculate AABB

		min = glm::vec3(min_x, min_y, min_z);
		max = glm::vec3(max_x, max_y, max_z);

		vector<Vertex> vertices;
		vector<int> indices;
		const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
		for (unsigned int j = 0; j < model[i]->mNumVertices; j++) {
			const aiVector3D* pPos = &(model[i]->mVertices[j]);
			const aiVector3D* pNormal = &(model[i]->mNormals[j]);
			const aiVector3D* pTexCoord = model[i]->HasTextureCoords(0) ? &(model[i]->mTextureCoords[0][j]) : &aiZeroVector;

			Vertex vert(
				glm::vec3(pPos->x, pPos->y, pPos->z),
				glm::vec2(pTexCoord->x, pTexCoord->y),
				glm::vec3(pNormal->x, pNormal->y, pNormal->z));
			vertices.push_back(vert);
		}

		for (unsigned int j = 0; j < model[i]->mNumFaces; j++) {
			const aiFace& face = model[i]->mFaces[j];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		initMesh(&vertices[0], vertices.size(), (int*)&indices[0], indices.size(), i, false);
	}
}

Mesh::~Mesh() {
	for (auto it = m_meshdata.begin(); it != m_meshdata.end(); it++)
		delete *it;
}

void Mesh::initMesh(Vertex* vertices, int vertSize, int* indices, int indexSize, int indexMesh, bool calcNormals){
	m_meshdata[indexMesh] = new MeshData(indexSize);

	if (calcNormals)
		this->calcNormals(vertices, vertSize, indices, indexSize);

	// Bind VAO
	glBindVertexArray(m_meshdata[indexMesh]->getVAO());
	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_meshdata[indexMesh]->getVBO());
	glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	// Bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshdata[indexMesh]->getIBO());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(int), indices, GL_STATIC_DRAW);
	
	// Set vertex attributes
	// Vertex coord
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	// Textures coord
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	// Normals coord
	glBindBuffer(GL_ARRAY_BUFFER, m_meshdata[indexMesh]->getVBO());
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::Draw() const {
	int numMesh = m_meshdata.size();

	for (int i = 0; i < numMesh; i++){
		glBindVertexArray(m_meshdata[i]->getVAO());
		glDrawElements(GL_TRIANGLES, m_meshdata[i]->getSize(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void Mesh::calcNormals(Vertex* vertices, int vertSize, int* indices, int indexSize){
	for (int i = 0; i < indexSize; i += 3)
	{
		int i0 = indices[i];
		int i1 = indices[i + 1];
		int i2 = indices[i + 2];

		glm::vec3 v1 = vertices[i1].vertices - vertices[i0].vertices;
		glm::vec3 v2 = vertices[i2].vertices - vertices[i0].vertices;

		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		vertices[i0].normal += normal;
		vertices[i1].normal += normal;
		vertices[i2].normal += normal;
	}

	for (int i = 0; i < vertSize; i++)
		vertices[i].normal = glm::normalize(vertices[i].normal);
}

