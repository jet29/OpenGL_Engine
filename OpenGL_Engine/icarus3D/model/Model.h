#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include "../Shader.h"

using std::vector;
using namespace glm;
using namespace std;

typedef enum { NEW_OBJECT, VCOORD, TCOORD, NCOORD, FACES, NONE } DATA_TYPE;

class Model {
	// Public variables
	public:
		vector<Model* > parts;
		vector<vec3> vCoord;
		vector<vec3> nCoord;
		vector<vec2> tCoord;
		vector<int> fCoord;
		string name;
		Shader* shader;
		unsigned int VAO, VBO, EBO;
		vec3 position = vec3(0,0,0);
		vec3 rotation = vec3(0,0,0);
		vec3 scale = vec3(1,1,1);
	// Private variables
	private:
	// Public functions
	public:
		Model();
		bool loadOBJ(const char* path);
		bool buildGeometry();
		void setShader(const char* vertexPath, const char* fragmentPath);
	// Private functions
	private:
		int getToken(string token);
};