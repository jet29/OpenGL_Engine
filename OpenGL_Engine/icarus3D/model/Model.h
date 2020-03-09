#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../Shader.h"
#include "Mesh.h"

using std::vector;
using namespace glm;
using namespace std;

class Model{
	// Public variables
	public:

	
		Mesh* mesh;
		std::string name;
		Shader* shader;
		vec3 position = vec3(0,0,0);
		vec3 scale = vec3(1,1,1);
		vec3 rotationAngles = glm::vec3(0.0f);
		vec3 pickingColor;
		mat4 rotationMatrix = glm::mat4(1.0f);
		mat4 translationMatrix = glm::mat4(1.0f);
		mat4 scalingMatrix = glm::mat4(1.0f);
		const char* shaderPath[2];
		mat4 modelMatrix = glm::mat4(1.0f);
	// Private variables
	private:
		GLuint BBVAO, BBVBO;
	// Public functions
	public:
		Model();
		~Model();

		void loadMesh(string path, string mtlPath);
		void setShader(const char* vertexPath, const char* fragmentPath);
		void buildBoundingBox();
		void DrawBoundingBox();
		void setRotationQuaternion(glm::vec3 eulerAngles);
		void setTranslationMatrix();
		void setScaleMatrix();
		glm::vec3 getEulerAnglesFromQuat();
		void computeModelMatrix();
};