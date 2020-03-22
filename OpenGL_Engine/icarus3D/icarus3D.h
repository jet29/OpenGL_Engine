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
#include <random>
// Lights
#include "light/DirectionalLight.h"
#include "light/PointLight.h"
#include "light/SpotLight.h"
#include <ImGuizmo.h>

//#include ""

//typedef sceneStruct icarusScene;
typedef enum { REGULAR_DEFERRED, SSAO } IC_FLAG;

typedef GLFWwindow ICwindow;
typedef unsigned int ICuint;

struct Stereoscopic {
	Camera* left_eye;
	Camera* right_eye;
};



class icarus3D {
	// Public variables
	public:
	vector<Scene *> scene;
	DirectionalLight *light;
	static Camera camera;
	static Stereoscopic stereoscopic;
	static bool cameraMode;
	static bool shiftBool;
	// Window current width
	static ICuint windowWidth;
	// Window current height
	static ICuint windowHeight;
	// Interface instance
	UI ui;
	int currentScene = -1;
	bool depthOfFieldBool = true;
	bool collisionBool;
	bool ssaoBool = true;
	float incremental = 1.0f;
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
		int gridLength;
		Shader* boundingBoxShader;
		Shader* deferredShader;
		Shader* deferredDepthShader;
		Shader* gridShader;
		Shader* skyboxShader;
		Shader* geometryPassShader;
		Shader* lightingPassShader;
		Shader* gBufferDebug;
		Shader* ssaoGeometryShader;
		Shader* ssaoLightingPass;
		Shader* ssaoShader;
		Shader* debugTextureShader;
		Shader* shaderSSAOBlur;
		Shader* stereoscopicShader;

		// Deferred Shading
		GLuint framebuffer, depthBuffer, gBuffer, pingPongFramebuffer;
		GLuint stereoscopic_left_eye_framebuffer, stereoscopic_right_eye_framebuffer, pingPongTexture;
		GLuint ssaoFBO, ssaoColorBuffer, ssaoBlurFBO, ssaoColorBufferBlur;
		GLuint gPosition, gNormal, gAlbedoSpec;
		GLuint DOFframebuffer, depthTexture, gDepth;
		GLuint fTexture, noiseTexture;
		GLuint leftEyeTexture, rightEyeTexture;
		GLuint cubemapTexture;
		GLuint kernel7, kernel11;
		unsigned int VBO, gridVBO;
		unsigned int VAO, gridVAO, skyboxVAO;
		unsigned int gridIBO;
		glm::mat4 gridModelMatrix = glm::mat4(1.0f);
		std::vector<glm::vec3> ssaoKernel;
	// Public functions
	public:
		//Method to obtain the only instance of the calls
		static icarus3D* Instance();
		void init();
		static unsigned int loadTexture(const char* path, int& texWidth, int& texHeight, int& numOfChannels);
		bool addModel(const string path, const string name="");
		bool addLight(const string path="");
		float inline getFPS() { return fps; }
		int inline getPickedIndex(){ return pickedIndex; }
		void inline setPickedIndex(int index) { pickedIndex = index; }
		bool createScene(string name = "");
		bool saveScene();
		bool loadScene(string path);
		unsigned int loadTexture(const char* path);

		void setLightingUniforms(Scene* scene, Shader* shader);
		void setDirectionalLightUniform(Scene* scene, Shader* shader);
		void setPointlightsUniform(Scene* scene, Shader* shader);
		// Private functions
	private:
		icarus3D();
		static void resize(ICwindow* window, int width, int height);
		static void onMouseMotion(ICwindow* window, double xpos, double ypos);
		static void onMouseButton(ICwindow* window, int button, int action, int mods);
		static void onKeyPress(ICwindow* window, int key, int scancode, int action, int mods);
		void render();
		void deferredShading();
		void drawBoundingBox();
		void renderToTexture();
		void forwardRendering();
		void depthOfField();
		void render2PassDeferredShading();
		void renderSSAO();
		void renderScene(Scene* scene);
		void renderSceneGeometryPass(Scene* scene, Shader* shader);
		void renderSceneLightingPass(Scene* scene);
		void renderSceneLightingPass(Scene* scene, Shader* shader);
		void renderPointlightModels();
		void renderBoundingBox();
		void renderStereoscopicViews();
		void drawGrid();
		void drawSkybox();
		bool initWindow();
		bool initGlad();
		void initGL();
		void initGrid();
		void initSkybox();
		bool initKernel();
		bool initSSAO();
		void initStereoscopicCameras();
		void duplicateTexture(GLuint target);
		void renderTexture(GLuint texture);
		void loadCubeMap(std::vector<std::string> faces);
		void processKeyboardInput(ICwindow* window);
		void updateFrames();
		bool checkCollision();
		void buildDeferredPlane();
		bool setFrameBuffer(GLuint& framebuffer, GLuint& texture);
		bool setSSAOFramebuffer();
		bool setFrameBufferDepth(GLuint& texture);
		bool setGeometryBuffer(IC_FLAG flag = REGULAR_DEFERRED);
		void pick();
		float lerp(float a, float b, float f) { return a + f * (b - a); }
};