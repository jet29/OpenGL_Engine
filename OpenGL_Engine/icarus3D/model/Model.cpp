#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Mesh class functions declarations

//std::map<string, MeshData*> Mesh::s_resourceMap;


Model::Model() {

}

Model::~Model() {

}

void Model::setShader(const char* vertexPath, const char* fragmentPath){
	delete shader;
	shader = new Shader(vertexPath, fragmentPath);
}

void Model::loadMesh(string path, string mtlPath) {

	mesh = new Mesh(path, mtlPath);
	buildBoundingBox();
}

void Model::buildBoundingBox() {
	vec3 v1 = vec3(this->mesh->max.x, this->mesh->max.y, this->mesh->max.z);
	vec3 v2 = vec3(this->mesh->max.x, this->mesh->max.y, this->mesh->min.z);
	vec3 v3 = vec3(this->mesh->max.x, this->mesh->min.y, this->mesh->max.z);
	vec3 v4 = vec3(this->mesh->max.x, this->mesh->min.y, this->mesh->min.z);
	vec3 v5 = vec3(this->mesh->min.x, this->mesh->min.y, this->mesh->min.z);
	vec3 v6 = vec3(this->mesh->min.x, this->mesh->min.y, this->mesh->max.z);
	vec3 v7 = vec3(this->mesh->min.x, this->mesh->max.y, this->mesh->max.z);
	vec3 v8 = vec3(this->mesh->min.x, this->mesh->max.y, this->mesh->min.z);

	//
	float boundingBox[] = {
		// Cara 1
		v1.x,v1.y,v1.z,
		v2.x,v2.y,v2.z,
		v4.x,v4.y,v4.z,
		v3.x,v3.y,v3.z,
		v1.x,v1.y,v1.z,
		v7.x,v7.y,v7.z,
		v8.x,v8.y,v8.z,
		v2.x,v2.y,v2.z,
		v4.x,v4.y,v4.z,
		v5.x,v5.y,v5.z,
		v8.x,v8.y,v8.z,
		v7.x,v7.y,v7.z,
		v6.x,v6.y,v6.z,
		v3.x,v3.y,v3.z,
		v6.x,v6.y,v6.z,
		v5.x,v5.y,v5.z,
	};

	glGenVertexArrays(1, &BBVAO);
	glGenBuffers(1, &BBVBO);
	glBindVertexArray(BBVAO);
	glBindBuffer(GL_ARRAY_BUFFER, BBVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boundingBox), &boundingBox, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Model::DrawBoundingBox(mat4 projectionMatrix, mat4 viewMatrix, mat4 modelMatrix) {
	// Binds the vertex array to be drawn
	glBindVertexArray(BBVAO);
	// Renders the geometry (fixed to 16)
	glDrawArrays(GL_LINE_STRIP, 0, 16);
	glBindVertexArray(0);
}

void Model::setRotationQuaternion(glm::vec3 eulerAngles) {
	rotationMatrix = glm::mat4(glm::quat(eulerAngles));
}

glm::vec3 Model::getEulerAnglesFromQuat() {
	return glm::eulerAngles(glm::toQuat(rotationMatrix));
}
