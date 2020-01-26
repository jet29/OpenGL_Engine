#pragma once
#include <iostream>
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
// Shader Class
#include "Shader.h"
#include "user_interface/userInterface.h"
#include "model/Model.h"
#include <vector>
//#include ""

//typedef sceneStruct icarusScene;

typedef GLFWwindow ICwindow;
typedef unsigned int ICuint;

class icarus3D {
	// Public variables
	public:
	std::vector<Model> models;
	// Private variables
	private:
	// Models array
	// Window context
	ICwindow* window;
	// Window current width
	ICuint windowWidth = 800;
	// Window current height
	ICuint windowHeight = 600;
	// Interface instance
	UI ui;
	// Public functions
	public:
	icarus3D();
	void init();
	static unsigned int loadTexture(const char* path, int& texWidth, int& texHeight, int& numOfChannels);
	bool addModel(std::vector<Model>& scene);
	// Private functions
	private:
	void resize(ICwindow* window, int width, int height);
	void render();
	bool initWindow();
	bool initGlad();
	void initGL();
};