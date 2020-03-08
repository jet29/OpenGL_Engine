#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

extern unsigned int blackTexture;
extern unsigned int whiteTexture;

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

Mesh::Mesh(const string& _path, const string &_mtlPath) {

	path = _path;
	mtlPath = _mtlPath;

	loadMaterials(mtlPath);

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path.c_str(),
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenSmoothNormals |
		aiProcess_GenBoundingBoxes);


	if (!scene) {
		cout << "Mesh load failed: " << path << endl;
		assert(0 != 0);
	}


	float max_x = INT_MIN;
	float max_y = INT_MIN;
	float max_z = INT_MIN;
	float min_x = INT_MAX;
	float min_y = INT_MAX;
	float min_z = INT_MAX;

	int numMeshes = scene->mNumMeshes;
	int numMaterials = scene->mNumMaterials;
	int numTextures = scene->mNumTextures;


	m_meshdata = std::vector<MeshData*>(numMeshes, NULL);
	vector<aiMesh*> model = vector<aiMesh*>(numMeshes, NULL);

	cout << "numMeshes: " << numMeshes << endl;
	cout << "numMaterials: " << numMaterials << endl;
	cout << "numTextures: " << numTextures << endl;

	for (int i = 0; i < numMaterials; i++) {
		cout << "name material: " << scene->mMaterials[i]->GetName().C_Str() << endl;
	}

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
				glm::vec3(pNormal->x, pNormal->y, pNormal->z)
			);

			vertices.push_back(vert);
		}

		for (unsigned int j = 0; j < model[i]->mNumFaces; j++) {
			
			const aiFace& face = model[i]->mFaces[j];
			
			assert(face.mNumIndices == 3);

			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		m_meshdata[i] = new MeshData(indices.size());

		m_meshdata[i]->material = materials[model[i]->mMaterialIndex];

		initMesh(&vertices[0], vertices.size(), (int*)&indices[0], indices.size(), i, false);
	}
}

Mesh::~Mesh() {
	for (auto it = m_meshdata.begin(); it != m_meshdata.end(); it++)
		delete *it;
}

void Mesh::initMesh(Vertex* vertices, int vertSize, int* indices, int indexSize, int indexMesh, bool calcNormals){
	


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

	// Normals coord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::Draw(Shader *shader) const {
	int numMesh = m_meshdata.size();

	for (int i = 0; i < numMesh; i++){

		shader->setInt("albedo", 0);
		glActiveTexture(GL_TEXTURE0);

		//cout << m_meshdata[i]->material->indexAlbedo << endl;
		if (m_meshdata[i]->material->indexAlbedo == INT_MAX)
			glBindTexture(GL_TEXTURE_2D, blackTexture);
		
		//set mesh material

		shader->setVec3("material.ka", m_meshdata[i]->material->ambient);
		shader->setVec3("material.kd", m_meshdata[i]->material->diffuse);
		shader->setVec3("material.ks", m_meshdata[i]->material->specular);
		shader->setFloat("material.shininess", m_meshdata[i]->material->shininess);


		glBindTexture(GL_TEXTURE_2D, m_meshdata[i]->material->indexAlbedo);


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


bool Mesh::loadMaterials(string path) {

	materials.push_back(new Material());

	FILE* file;

	int err = fopen_s(&file, path.c_str(), "r");
	if (err != 0) {
		printf("Impossible to open the file !\n");
		return false;
	}

	int currentMaterial = 0;

	while (1) {

		char lineHeader[1000];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// if found new material
		if (strcmp(lineHeader, "newmtl") == 0) {

			char name[1000];
			fscanf(file, "%s \n", &name);
			
			Material *material = new Material();
			
			materials.push_back(material);

			currentMaterial++;
		}
		else if (strcmp(lineHeader, "Ns") == 0) {

			float ns;
			fscanf(file, "%f \n", &ns);

			materials[currentMaterial]->shininess = ns;
		}
		else if (strcmp(lineHeader, "Ka") == 0) {
			glm::vec3 ka;

			fscanf(file, "%f %f %f\n", &ka.x, &ka.y, &ka.z);
		
			materials[currentMaterial]->ambient = ka;
		}
		else if (strcmp(lineHeader, "Kd") == 0) {
			glm::vec3 kd;

			fscanf(file, "%f %f %f\n", &kd.x, &kd.y, &kd.z);

			materials[currentMaterial]->diffuse = kd;
		}
		else if (strcmp(lineHeader, "Ks") == 0) {
			glm::vec3 ks;

			fscanf(file, "%f %f %f\n", &ks.x, &ks.y, &ks.z);

			materials[currentMaterial]->specular = ks;
		}
		else if (strcmp(lineHeader, "illum") == 0) {
			int ilum;

			fscanf(file, "%i \n", &ilum);

			materials[currentMaterial]->ilum = ilum;
		}
		else if (strcmp(lineHeader, "d") == 0) {

			float d;
			fscanf(file, "%f \n", &d);

			materials[currentMaterial]->alpha = d;
		}
		else if (strcmp(lineHeader, "map_Kd") == 0) {

			char path[1000];
			fscanf(file, "%s \n", &path);

			materials[currentMaterial]->indexAlbedo = loadTexture(path);

		}

	}

	cout << "Materials loaded successfully" << endl;

	return true;
}

unsigned int Mesh::loadTexture(const char* path)
{
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Loads the texture
	int textureWidth, textureHeight, numberOfChannels;
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);
	// Loads the texture file data
	unsigned char* data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
	if (data)
	{
		// Gets the texture channel format
		GLenum format;
		switch (numberOfChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		// Binds the texture
		glBindTexture(GL_TEXTURE_2D, id);
		// Creates the texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
		// Creates the texture mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);

	return id;
}

