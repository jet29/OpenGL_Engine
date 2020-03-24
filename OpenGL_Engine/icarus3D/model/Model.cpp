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

void Model::DrawBoundingBox() {
	// Binds the vertex array to be drawn
	glBindVertexArray(BBVAO);
	// Renders the geometry (fixed to 16)
	glDrawArrays(GL_LINE_STRIP, 0, 16);
	glBindVertexArray(0);
}

float LengthSq(glm::vec3 a){ return (a.x * a.x + a.y * a.y + a.z * a.z); };

void rotationAxis(glm::vec3 axis,glm::mat4& rot, float angle) {


	glm::vec3 n = axis * (1.f / sqrtf(1));
	float s = sinf(angle);
	float c = cosf(angle);
	float k = 1.f - c;

	float xx = n.x * n.x * k + c;
	float yy = n.y * n.y * k + c;
	float zz = n.z * n.z * k + c;
	float xy = n.x * n.y * k;
	float yz = n.y * n.z * k;
	float zx = n.z * n.x * k;
	float xs = n.x * s;
	float ys = n.y * s;
	float zs = n.z * s;

	rot[0][0] = xx;
	rot[0][1] = xy + zs;
	rot[0][2] = zx - ys;
	rot[0][3] = 0.f;
	rot[1][0] = xy - zs;
	rot[1][1] = yy;
	rot[1][2] = yz + xs;
	rot[1][3] = 0.f;
	rot[2][0] = zx + ys;
	rot[2][1] = yz - xs;
	rot[2][2] = zz;
	rot[2][3] = 0.f;
	rot[3][0] = 0.f;
	rot[3][1] = 0.f;
	rot[3][2] = 0.f;
	rot[3][3] = 1.f;
}

void Model::setRotationQuaternion(glm::vec3 eulerAngles) {
	vec3 angles = glm::radians(eulerAngles);
	//rotationMatrix = glm::mat4(glm::quat(glm::radians(eulerAngles)));
	glm::mat4 rot[3];
	vector<glm::vec3> axis;
	axis.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	axis.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	axis.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	for (int i = 0; i < 3; i++)
		rotationAxis(axis[i], rot[i], angles[i]);
	rotationMatrix  = rot[0] * rot[1] * rot[2];
}


glm::vec3 Model::getEulerAnglesFromQuat() {
	return glm::eulerAngles(glm::toQuat(rotationMatrix));
}

void Model::setTranslationMatrix() {
	translationMatrix = glm::translate(glm::mat4(1.0f), position);
}

void Model::setScaleMatrix() {
	scalingMatrix = glm::scale(glm::mat4(1.0f), scale);
}

void Model::computeModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix *= translationMatrix;
	modelMatrix *= scalingMatrix;
	modelMatrix *= rotationMatrix;
}

