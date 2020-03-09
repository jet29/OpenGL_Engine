#pragma once
#include <iostream>
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
// Shader Class
#include "Shader.h"
#include "user_interface/userInterface.h"
#include "model/Model.h"
#include "model/Scene.h"
#include "camera/Camera.h"
#include <vector>
// Lights
#include "light/DirectionalLight.h"
#include "light/PointLight.h"
#include "light/SpotLight.h"

//#include ""

//typedef sceneStruct icarusScene;

typedef GLFWwindow ICwindow;
typedef unsigned int ICuint;



class icarus3D {
	// Public variables
	public:
	vector<Scene *> scene;
	DirectionalLight *light;
	static Camera camera;
	static bool cameraMode;
	// Window current width
	ICuint windowWidth = 800;
	// Window current height
	ICuint windowHeight = 600;
	// Interface instance
	UI ui;
	int currentScene = -1;

	// Private variables
	private:
		//Holds the instance of the class
		static icarus3D* instance; 
		// Window context
		ICwindow* window;
		// Picking shader
		Shader *pickingShader;
		// Windows FPS
		float currentTime = 0;
		float lastTime = 0;
		float fps = 0;
		float totalTime = 0;
		float deltaTime;
		int totalFrames = 0;
		int pickedIndex = -1;
		Shader* boundingBox;
	// Public functions
	public:
		//Method to obtain the only instance of the calls
		static icarus3D* Instance();
		void init();
		static unsigned int loadTexture(const char* path, int& texWidth, int& texHeight, int& numOfChannels);
		bool addModel(const string path);
		float inline getFPS() { return fps; }
		int inline getPickedIndex(){ return pickedIndex; }
		void inline setPickedIndex(int index) { pickedIndex = index; }
		bool createScene();
		bool saveScene();
		bool loadScene(string path);
		unsigned int loadTexture(const char* path);
		// Private functions
	private:
		icarus3D();
		void resize(ICwindow* window, int width, int height);
		static void onMouseMotion(ICwindow* window, double xpos, double ypos);
		static void onMouseButton(ICwindow* window, int button, int action, int mods);
		static void onKeyPress(ICwindow* window, int key, int scancode, int action, int mods);
		void render();
		void drawBoundingBox();
		void renderScene(Scene *scene);
		bool initWindow();
		bool initGlad();
		void initGL();
		void processKeyboardInput(GLFWwindow* window);
		void updateFrames();
		bool checkCollision(Scene *scene);
		void pick();

};