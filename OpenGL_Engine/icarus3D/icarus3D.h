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
// Particle System
#include "particle_system/particleSystem.h"
#include "particle_system/particle.h"

//typedef sceneStruct icarusScene;
typedef GLFWwindow ICwindow;
typedef unsigned int ICuint;

struct Stereoscopic {
	Camera* left_eye;
	Camera* right_eye;
	float convergence = 7.0f;
	float IOD = 1.0f;
	float fov = 45.0f;
	glm::vec4 le_color = glm::vec4(1.0f,0.0f,0.0f,0.5f);
	glm::vec4 re_color = glm::vec4(0.0f, 0.0f, 1.0f,0.5f);
};

struct SSAO {
	int kernelSize = 64;
	float radius = 0.5;
	float bias = 0.025;
	std::vector<glm::vec3> kernel;
};



class icarus3D {
	// Public variables
	public:
	vector<Scene *> scene;
	DirectionalLight *light;
	ParticleSystem* particleSystem;
	static Camera camera;
	static Stereoscopic stereoscopic;
	static SSAO ssao;
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
	int particleSystemSeed = 20;
	bool DoFBool = false;
	bool collisionBool;
	bool ssaoBool = true;
	bool particlesystemBool = false;
	bool stereoBool = false;
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
		Shader* ssaoLightingPassShader;
		Shader* ssaoShader;
		Shader* debugTextureShader;
		Shader* shaderSSAOBlur;
		Shader* stereoscopicShader;
		Shader* particleSystemShader;

		// Deferred Shading
		GLuint framebuffer, depthBuffer, gBuffer, pingPongFramebuffer;
		GLuint stereoscopic_left_eye_framebuffer, stereoscopic_right_eye_framebuffer, pingPongTexture;
		GLuint ssaoFBO, ssaoColorBuffer, ssaoBlurFBO, ssaoColorBufferBlur;
		GLuint gPosition, gNormal, gAlbedoSpec;
		GLuint DOFframebuffer, depthTexture, gDepth;
		GLuint fTexture, noiseTexture;
		GLuint leftEyeTexture, rightEyeTexture;
		GLuint cubemapTexture;
		GLuint particleSystemTexture;
		GLuint kernel7, kernel11;
		unsigned int VBO, gridVBO;
		unsigned int VAO, gridVAO, skyboxVAO;
		unsigned int gridIBO;
		glm::mat4 gridModelMatrix = glm::mat4(1.0f);

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
		bool saveScene(string path);
		bool loadScene(string path, string name);
		unsigned int loadTexture(const char* path);

		void setLightingUniforms(Scene* scene, Shader* shader);
		void setDirectionalLightUniform(Scene* scene, Shader* shader);
		void setPointlightsUniform(Scene* scene, Shader* shader);
		void updateStereoPerspectiveMatrix();
		void computeSSAOKernel();
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
		void depthOfField();
		void renderSSAO();
		void renderScene(Scene* scene);
		void renderSceneGeometryPass(Scene* scene, Shader* shader);
		void renderSceneLightingPass(Scene* scene, Shader* shader);
		void renderPointlightModels();
		void renderBoundingBox();
		void renderStereoscopicViews();
		void renderParticleSystem();
		void drawGrid();
		void drawSkybox();
		bool initWindow();
		bool initGlad();
		void initGL();
		void initGrid();
		void initSkybox();
		bool initKernel();
		bool initSSAO();
		bool initFramebuffers();
		void initStereoscopicCameras();
		void duplicateTexture(GLuint target);
		void renderTexture(GLuint texture);
		void loadCubeMap(std::vector<std::string> faces);
		void processKeyboardInput(ICwindow* window);
		void updateFrames();
		bool checkCollision();
		void buildQuad();
		bool setFrameBuffer(GLuint& framebuffer, GLuint& texture);
		bool setSSAOFramebuffer();
		bool setGeometryBuffer();
		void pick();
		float lerp(float a, float b, float f) { return a + f * (b - a); }
};