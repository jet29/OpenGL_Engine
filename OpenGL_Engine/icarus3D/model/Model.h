#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
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
		string name;
		Shader* shader;
		vec3 position = vec3(0,0,0);
		vec3 rotation = vec3(0,0,0);
		vec3 scale = vec3(1,1,1);
	// Private variables
	private:
	// Public functions
	public:
		Model();
		void loadMesh(string path);
		void setShader(const char* vertexPath, const char* fragmentPath);
};