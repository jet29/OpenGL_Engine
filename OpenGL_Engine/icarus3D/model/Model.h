#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using std::vector;
using namespace glm;
using namespace std;

typedef enum { NEW_OBJECT, VCOORD, TCOORD, NCOORD, FACES, NONE } DATA_TYPE;

class Model {
	// Public variables
	public:
		vector<Model> parts;
		vector<vec3> vCoord;
		vector<vec3> nCoord;
		vector<vec2> tCoord;
		string name;
		unsigned int VAO, VBO;
	// Private variables
	private:
	// Public functions
	public:
		Model();
		bool loadOBJ(const char* path);
	// Private functions
	private:
		int getToken(string token);
};