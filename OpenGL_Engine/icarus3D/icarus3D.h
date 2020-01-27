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
#include "camera/Camera.h"
#include <vector>
//#include ""

//typedef sceneStruct icarusScene;

typedef GLFWwindow ICwindow;
typedef unsigned int ICuint;

class icarus3D {
	// Public variables
	public:
	std::vector<Model> models;
	static Camera camera;
	// Private variables
	private:
		//Holds the instance of the class
		static icarus3D* instance; 
		// Window context
		ICwindow* window;
		// Window current width
		ICuint windowWidth = 800;
		// Window current height
		ICuint windowHeight = 600;
		// Delta Time
		float currentTime = 0;
		float lastTime = 0;
		// Interface instance
		UI ui;

	// Public functions
	public:
		//Method to obtain the only instance of the calls
		static icarus3D* Instance();
		void init();
		static unsigned int loadTexture(const char* path, int& texWidth, int& texHeight, int& numOfChannels);
		bool addModel(std::vector<Model>& scene);
		// Private functions
	private:
		icarus3D();
		void resize(ICwindow* window, int width, int height);
		static void onMouseMotion(ICwindow* window, double xpos, double ypos);
		void render();
		void renderScene(std::vector<Model>& scene);
		bool initWindow();
		bool initGlad();
		void initGL();
		void processKeyboardInput(GLFWwindow* window);
};