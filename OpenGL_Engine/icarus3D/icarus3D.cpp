﻿#define _CRT_SECURE_NO_WARNINGS

#include "icarus3D.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Static Variables
ICuint icarus3D::windowWidth = 800;
ICuint icarus3D::windowHeight = 600;

// Global static pointer used to ensure a single instance of the class.
icarus3D* icarus3D::instance = NULL;
Camera icarus3D::camera(windowWidth, windowHeight);
Stereoscopic icarus3D::stereoscopic;
SSAO icarus3D::ssao;
bool icarus3D::cameraMode = false;
bool icarus3D::shiftBool = false;

// Textures
unsigned int whiteTexture;
unsigned int blackTexture;

/**
* Creates an instance of the class
*
* @return the instance of this class
*/
icarus3D* icarus3D::Instance() {
	if (!instance)   // Only allow one instance of class to be generated.
		instance = new icarus3D();
	return instance;
}

icarus3D::icarus3D() {
	
}

// Public Functions
unsigned int icarus3D::loadTexture(const char* path, int &texWidth, int &texHeight, int &numOfChannels)
{
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);

	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);
	// Loads the texture file data
	unsigned char* data = stbi_load(path, &texWidth, &texHeight, &numOfChannels, 0);
	if (data)
	{
		// Gets the texture channel format
		GLenum format;
		switch (numOfChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		// Binds the texture
		glBindTexture(GL_TEXTURE_2D, id);
		// Creates the texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
		// Creates the texture mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);

	return id;
}

void icarus3D::init() {
	// Initialize window and glad components
	if (!initWindow() || !initGlad() || !initFramebuffers()) {
		std::cout << "ERROR::Couldn't initialize window or GLAD components" << std::endl;
		return;
	}
	// Initialize OpenGL context
	initGL();

	// Initialize user interface context
	ui.init(window);

	// Initialize stereoscopic cameras
	initStereoscopicCameras();
	
	// Initialize grid data
	initGrid();

	// Init DIP kernels
	initKernel();

	// Init SSAO kernel and noise texture
	initSSAO();

	// init skybox
	initSkybox();

	// Init shaders
	pickingShader = new Shader("icarus3D/shaders/picking.vert", "icarus3D/shaders/picking.frag");
	boundingBoxShader = new Shader("icarus3D/shaders/bounding_box.vert", "icarus3D/shaders/bounding_box.frag");
	deferredShader = new Shader("icarus3D/shaders/deferred.vert", "icarus3D/shaders/deferred.frag");
	deferredDepthShader = new Shader("icarus3D/shaders/deferredDepth.vert", "icarus3D/shaders/deferredDepth.frag");
	gridShader = new Shader("icarus3D/shaders/gridShader.vert", "icarus3D/shaders/gridShader.frag");
	skyboxShader = new	Shader("icarus3D/shaders/skyboxShader.vert","icarus3D/shaders/skyboxShader.frag");
	geometryPassShader = new Shader("icarus3D/shaders/geometryPassShader.vert", "icarus3D/shaders/geometryPassShader.frag");
	lightingPassShader = new Shader("icarus3D/shaders/lightingPassShader.vert", "icarus3D/shaders/lightingPassShader.frag");
	ssaoLightingPassShader = new Shader("icarus3D/shaders/ssao/ssaoLightingPassShader.vert", "icarus3D/shaders/ssao/ssaoLightingPassShader.frag");
	debugTextureShader = new Shader("icarus3D/shaders/texture_debug.vert", "icarus3D/shaders/texture_debug.frag");
	ssaoGeometryShader = new Shader("icarus3D/shaders/ssao/ssao_geometry.vert", "icarus3D/shaders/ssao/ssao_geometry.frag");
	ssaoShader = new Shader("icarus3D/shaders/ssao/ssao.vert", "icarus3D/shaders/ssao/ssao.frag");
	shaderSSAOBlur = new Shader("icarus3D/shaders/ssao/ssao_blur.vert", "icarus3D/shaders/ssao/ssao_blur.frag");
	stereoscopicShader = new Shader("icarus3D/shaders/stereoscopic_shader.vert", "icarus3D/shaders/stereoscopic_shader.frag");
	particleSystemShader = new Shader("icarus3D/shaders/particleSystem.vert", "icarus3D/shaders/particleSystem.frag");
	
	// Load default textures
	whiteTexture = loadTexture("assets/textures/white_bg.png");
	blackTexture = loadTexture("assets/textures/black_bg.png");
	particleSystemTexture = loadTexture("assets/textures/rain2.png");

	buildQuad();

	// Create Mandatory Dir Light
	light = new DirectionalLight();

	// Create Particle System
	particleSystem = new ParticleSystem(VAO, VBO, particleSystemSeed);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Begin render loop
	render();
	// Terminate interface instance
	ui.terminate();
	// Terminate GLFW, clearing any resources allocated by GLFW.s
	glfwTerminate();
}

bool icarus3D::initFramebuffers() {
	bool flag = setFrameBuffer(framebuffer, fTexture) &&
				setGeometryBuffer() &&
				setSSAOFramebuffer() &&
				setFrameBuffer(stereoscopic_left_eye_framebuffer, leftEyeTexture) &&
				setFrameBuffer(stereoscopic_right_eye_framebuffer, rightEyeTexture) &&
				setFrameBuffer(pingPongFramebuffer, pingPongTexture);

	return flag;
}

void icarus3D::initSkybox() {
	// Cubemap and Skybox initialization
	std::vector<std::string> faces{
		"assets/textures/skybox/default/right.jpg",
		"assets/textures/skybox/default/left.jpg",
		"assets/textures/skybox/default/top.jpg",
		"assets/textures/skybox/default/bottom.jpg",
		"assets/textures/skybox/default/front.jpg",
		"assets/textures/skybox/default/back.jpg"
	};

	// Load cubemap texture
	loadCubeMap(faces);
	
	// Skybox vertices
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// Load skybox into GPU
	unsigned int skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void icarus3D::initGrid() {
	std::vector<glm::vec3> vertices;
	std::vector<glm::uvec4> indices;

	gridModelMatrix = glm::scale(gridModelMatrix, glm::vec3(400.0, 1.0f, 400.0f));
	int slices = 30;

	for (int j = 0; j <= slices; ++j) {
		for (int i = 0; i <= slices; ++i) {
			float x = (float)i / (float)slices;
			float y = 0;
			float z = (float)j / (float)slices;
			vertices.push_back(glm::vec3(x - 0.5f, y, z - 0.5f));
		}
	}

	for (int j = 0; j < slices; ++j) {
		for (int i = 0; i < slices; ++i) {

			int row1 = j * (slices + 1);
			int row2 = (j + 1) * (slices + 1);

			indices.push_back(glm::uvec4(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1));
			indices.push_back(glm::uvec4(row2 + i + 1, row2 + i, row2 + i, row1 + i));

		}
	}

	glGenVertexArrays(1, &gridVAO);
	glBindVertexArray(gridVAO);
	glGenBuffers(1, &gridVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &gridIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec4), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	gridLength = (GLuint)indices.size() * 4;
}

void icarus3D::initStereoscopicCameras() {
	stereoscopic.left_eye = new Camera(windowWidth, windowHeight);
	stereoscopic.right_eye = new Camera(windowWidth, windowHeight);

	// Attempt 2 - Code based on paper: https://cgvr.cs.uni-bremen.de/teaching/vr_literatur/Rendering%203D%20Anaglyph%20in%20OpenGL.pdf
	// Left Eye
	float convergence = 7.0f;
	// Intraocular distance
	float IOD = 1.0f;
	float aspectRatio = (float)windowWidth / (float)windowHeight;
	float nearClippingDistance = 1.0f;
	float farClippingDistance = 100.0f;
	float fov = glm::radians(45.0f);

	float top, bottom, left, right;
	top = nearClippingDistance * glm::tan(fov / 2);
	bottom = -top;
	float a = aspectRatio * tan(fov / 2) * convergence;
	float b = a - IOD / 2;
	float c = a + IOD / 2;
	left = -b * nearClippingDistance / convergence;
	right = c * nearClippingDistance / convergence;

	stereoscopic.left_eye->viewDirection = vec3(0, 0, -1);
	glm::vec3 moveDirection = glm::cross(stereoscopic.left_eye->viewDirection, stereoscopic.left_eye->UP);
	stereoscopic.left_eye->position += -IOD * moveDirection;
	stereoscopic.left_eye->viewMatrix = glm::lookAt(stereoscopic.left_eye->position, stereoscopic.left_eye->position + stereoscopic.left_eye->viewDirection, stereoscopic.left_eye->UP);
	stereoscopic.left_eye->perspectiveMatrix = glm::frustum(left,right,bottom,top,nearClippingDistance,farClippingDistance);

	// Right eye
	top = nearClippingDistance * glm::tan(fov / 2);
	bottom = -top;
	a = aspectRatio * tan(fov / 2) * convergence;
	b = a - IOD / 2;
	c = a + IOD / 2;
	left = -c * nearClippingDistance / convergence;
	right = b * nearClippingDistance / convergence;

	stereoscopic.right_eye->viewDirection = vec3(0, 0, -1);
	moveDirection = glm::cross(stereoscopic.right_eye->viewDirection, stereoscopic.right_eye->UP);
	stereoscopic.right_eye->position += IOD * moveDirection;
	stereoscopic.right_eye->viewMatrix = glm::lookAt(stereoscopic.right_eye->position, stereoscopic.right_eye->position + stereoscopic.right_eye->viewDirection, stereoscopic.right_eye->UP);
	stereoscopic.right_eye->perspectiveMatrix = glm::frustum(left, right, bottom, top, nearClippingDistance, farClippingDistance);

} 

void icarus3D::updateStereoPerspectiveMatrix() {
	float aspectRatio = (float)windowWidth / (float)windowHeight;
	float nearClippingDistance = 1.0f;
	float farClippingDistance = 100.0f;
	float fov = glm::radians(stereoscopic.fov);

	// Update left eye
	float top, bottom, left, right;
	top = nearClippingDistance * glm::tan(fov / 2);
	bottom = -top;
	float a = aspectRatio * tan(fov / 2) * stereoscopic.convergence;
	float b = a - stereoscopic.IOD / 2;
	float c = a + stereoscopic.IOD / 2;
	left = -b * nearClippingDistance / stereoscopic.convergence;
	right = c * nearClippingDistance / stereoscopic.convergence;

	stereoscopic.left_eye->perspectiveMatrix = glm::frustum(left, right, bottom, top, nearClippingDistance, farClippingDistance);

	// Update right eye
	top = nearClippingDistance * glm::tan(fov / 2);
	bottom = -top;
	a = aspectRatio * tan(fov / 2) * stereoscopic.convergence;
	b = a - stereoscopic.IOD / 2;
	c = a + stereoscopic.IOD / 2;
	left = -c * nearClippingDistance / stereoscopic.convergence;
	right = b * nearClippingDistance / stereoscopic.convergence;
	stereoscopic.right_eye->perspectiveMatrix = glm::frustum(left, right, bottom, top, nearClippingDistance, farClippingDistance);

}

// Private Functions
bool icarus3D::initWindow(){
	// Initialize glfw
	glfwInit();
	// Sets the Opegl context to Opengl 3.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates the window
	window = glfwCreateWindow(windowWidth, windowHeight, "ICARUS3D ENGINE", NULL, NULL);

	// The window couldn't be created
	if (!window)
	{
		std::cout << "ERROR::Failed to create the glfw window" << std::endl;
		glfwTerminate(); // Stops the glfw program
		return false;
	}

	// Creates the glfwContext, this has to be made before calling initGlad()
	glfwMakeContextCurrent(window);

	// Window resize callback
	glfwSetFramebufferSizeCallback(window, resize);
	// Mouse position callback
	glfwSetCursorPosCallback(window, onMouseMotion);
	// Mouse buttons callback
	glfwSetMouseButtonCallback(window, onMouseButton);
	// Keyboard buttons callback
	glfwSetKeyCallback(window, onKeyPress);
	return true;
}

bool icarus3D::initGlad() {
	// Initialize glad
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// If something went wrong during the glad initialization
	if (!status)
	{
		std::cout << status << std::endl;
		std::cout << "ERROR::Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

void icarus3D::initGL() {
	// Enables the z-buffer test
	glEnable(GL_DEPTH_TEST);
	// Sets the ViewPort
	glViewport(0, 0, windowWidth, windowHeight);
	// Sets the clear color
	glClearColor(0.78f, 0.78f, 0.78f, 1.0f);
}

void icarus3D::loadCubeMap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	cubemapTexture = textureID;
}

void icarus3D::processKeyboardInput(ICwindow* window)
{
	float MOVEMENT_SPEED = 10.0f;
	float speed = MOVEMENT_SPEED * deltaTime;
	// Checks if the escape key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// Tells glfw to close the window as soon as possible
		glfwSetWindowShouldClose(window, true);

	if (cameraMode) {
		// Move Forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.moveForward(deltaTime);
			stereoscopic.left_eye->moveForward(deltaTime);
			stereoscopic.right_eye->moveForward(deltaTime);
		}
		// Move Backward
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.moveBackward(deltaTime);
			stereoscopic.left_eye->moveBackward(deltaTime);
			stereoscopic.right_eye->moveBackward(deltaTime);
		}
		// Move right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.moveRight(deltaTime);
			stereoscopic.left_eye->moveRight(deltaTime);
			stereoscopic.right_eye->moveRight(deltaTime);
		}
		// Move left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.moveLeft(deltaTime);
			stereoscopic.left_eye->moveLeft(deltaTime);
			stereoscopic.right_eye->moveLeft(deltaTime);
		}

		// Move Up
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.moveUp(deltaTime);
			stereoscopic.left_eye->moveUp(deltaTime);
			stereoscopic.right_eye->moveUp(deltaTime);
		}

		// Move Down
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.moveDown(deltaTime);
			stereoscopic.left_eye->moveDown(deltaTime);
			stereoscopic.right_eye->moveDown(deltaTime);
		}
	}


}

void icarus3D::onKeyPress(ICwindow* window, int key, int scancode, int action, int mods) {

	// Actions when camera mode is disabled
	if (action == GLFW_PRESS && !cameraMode) {

		if (mods == GLFW_MOD_SHIFT) shiftBool = true;

		switch (key) {
		case GLFW_KEY_F:
			if (mods == GLFW_MOD_SHIFT) {
				printf("Camera mode activated\n");
				cameraMode = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPos(window, instance->windowWidth / 2.0, instance->windowHeight / 2.0);
			}
			break;
		case GLFW_KEY_R:
			if (instance->currentScene != -1 && instance->scene[instance->currentScene]->models.size() != 0)
				for (auto &model : instance->scene[instance->currentScene]->models)
					model->setShader(model->shaderPath[0].c_str(), model->shaderPath[1].c_str());

			delete instance->pickingShader;
			delete instance->boundingBoxShader;
			delete instance->deferredShader;
			delete instance->deferredDepthShader;
			delete instance->gridShader;
			delete instance->geometryPassShader;
			delete instance->lightingPassShader;
			delete instance->ssaoLightingPassShader;
			delete instance->ssaoGeometryShader;
			delete instance->ssaoShader;
			delete instance->debugTextureShader;
			delete instance->shaderSSAOBlur;
			delete instance->stereoscopicShader;

			instance->gridShader = new Shader("icarus3D/shaders/gridShader.vert", "icarus3D/shaders/gridShader.frag");
			instance->pickingShader = new Shader("icarus3D/shaders/picking.vert", "icarus3D/shaders/picking.frag");
			instance->boundingBoxShader = new Shader("icarus3D/shaders/bounding_box.vert", "icarus3D/shaders/bounding_box.frag");
			instance->deferredShader = new Shader("icarus3D/shaders/deferred.vert", "icarus3D/shaders/deferred.frag");
			instance->deferredDepthShader = new Shader("icarus3D/shaders/deferredDepth.vert", "icarus3D/shaders/deferredDepth.frag");
			instance->geometryPassShader = new Shader("icarus3D/shaders/geometryPassShader.vert", "icarus3D/shaders/geometryPassShader.frag");
			instance->lightingPassShader = new Shader("icarus3D/shaders/lightingPassShader.vert", "icarus3D/shaders/lightingPassShader.frag");
			instance->ssaoLightingPassShader = new Shader("icarus3D/shaders/ssao/ssaoLightingPassShader.vert", "icarus3D/shaders/ssao/ssaoLightingPassShader.frag");
			instance->debugTextureShader = new Shader("icarus3D/shaders/texture_debug.vert", "icarus3D/shaders/texture_debug.frag");
			instance->ssaoGeometryShader = new Shader("icarus3D/shaders/ssao/ssao_geometry.vert", "icarus3D/shaders/ssao/ssao_geometry.frag");
			instance->ssaoShader = new Shader("icarus3D/shaders/ssao/ssao.vert", "icarus3D/shaders/ssao/ssao.frag");
			instance->shaderSSAOBlur = new Shader("icarus3D/shaders/ssao/ssao_blur.vert", "icarus3D/shaders/ssao/ssao_blur.frag");
			instance->stereoscopicShader = new Shader("icarus3D/shaders/stereoscopic_shader.vert", "icarus3D/shaders/stereoscopic_shader.frag");

			break;
		}
	}
	// Actions when camera mode is activated
	else if (action == GLFW_PRESS && cameraMode) {
		switch (key) {
		case GLFW_KEY_F:
			if (mods == GLFW_MOD_SHIFT) {
				printf("Camera mode deactivated\n");
				cameraMode = false;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			break;
		}
	}
	
	if (mods != GLFW_MOD_SHIFT) shiftBool = false;

	
}

void icarus3D::onMouseMotion(ICwindow* window, double xpos, double ypos)
{
	if (cameraMode){
		glfwSetCursorPos(window, instance->windowWidth / 2.0, instance->windowHeight / 2.0);
		double xoffset = ((instance->windowWidth / 2.0) - xpos) * camera.mouseSpeed * instance->deltaTime;
		double yoffset = ((instance->windowHeight / 2.0) - ypos) * camera.mouseSpeed * instance->deltaTime;

		camera.mouseUpdate(glm::vec2(xoffset, yoffset));

		stereoscopic.left_eye->mouseUpdate(glm::vec2(xoffset, yoffset));
		stereoscopic.right_eye->mouseUpdate(glm::vec2(xoffset, yoffset));
	}
}

void icarus3D::onMouseButton(ICwindow* window, int button, int action, int mods){

	if (action == GLFW_PRESS && instance->currentScene != -1 && instance->shiftBool)
		instance->pick();
}

void icarus3D::resize(ICwindow* window, int width, int height){
	windowWidth = width;
	windowHeight = height;
	// Sets the OpenGL viewport size and position
	glViewport(0, 0, windowWidth, windowHeight);

	//camera.resize(windowWidth, windowHeight);
	//instance->setFrameBuffer(instance->dsTexture);
	instance->setSSAOFramebuffer();
}

void icarus3D::renderScene(Scene *scene) {
	// Iterate over scene models
	for (auto& model : scene->models) {
		
		// Use a single shader per model
		model->shader->use();
	
		if (model->type == MODEL) {
		// Set Directional light shader uniforms
		model->shader->setVec3("dirlight.direction", light->properties.direction);
		model->shader->setVec3("dirlight.color.ambient", light->properties.color.ambient);
		model->shader->setVec3("dirlight.color.diffuse", light->properties.color.diffuse);
		model->shader->setVec3("dirlight.color.specular", light->properties.color.specular);
		model->shader->setBool("dirlight.lightSwitch", light->lightSwitch);
			// Set Point lights shader uniforms
			model->shader->setInt("numOfPointLight", scene->pointlight_index.size());
			for (ICuint i = 0; i < scene->pointlight_index.size(); i++) {
				PointLight* pointlight = (PointLight*)scene->models[scene->pointlight_index[i]];
				std::string index = std::to_string(i);
				// Set pointlight position
				model->shader->setVec3("pointlight[" + index + "].position", pointlight->position);
				// Set pointlight color
				model->shader->setVec3("pointlight[" + index + "].color.ambient", pointlight->properties.color.ambient);
				model->shader->setVec3("pointlight[" + index + "].color.diffuse", pointlight->properties.color.diffuse);
				model->shader->setVec3("pointlight[" + index + "].color.specular", pointlight->properties.color.specular);
				// Set pointlight attenuationF
				model->shader->setFloat("pointlight[" + index + "].attenuation.constant", pointlight->properties.attenuation.constant);
				model->shader->setFloat("pointlight[" + index + "].attenuation.linear", pointlight->properties.attenuation.linear);
				model->shader->setFloat("pointlight[" + index + "].attenuation.quadratic", pointlight->properties.attenuation.quadratic);
				// Set pointlight switch bool
				model->shader->setBool("pointlight[" + index + "].lightSwitch", pointlight->lightSwitch);
				// Set directional light switch bool
			}
		}
		else if (model->type == POINTLIGHT) {
			PointLight* pointlight = (PointLight*)model;
			model->shader->setVec3("ambient", pointlight->properties.color.ambient);
			model->shader->setVec3("diffuse", pointlight->properties.color.diffuse);
			model->shader->setVec3("specular", pointlight->properties.color.specular);
			model->shader->setBool("lightSwitch", pointlight->lightSwitch);
		}

		// Set model shader uniforms
		model->shader->setVec3("viewPos", camera.position);
		model->shader->setMat4("model", model->modelMatrix);
		model->shader->setMat4("view", camera.viewMatrix);
		model->shader->setMat4("projection", camera.getPerspectiveMatrix());
	
		// Render model
		model->mesh->Draw(model->shader);

	}
}

void icarus3D::renderSceneGeometryPass(Scene* scene, Shader* shader) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render geometry only first into MRT
	shader->use();
	// Iterate over scene models
	for (auto& model : scene->models) {
		// Ignore pointlight models
		if (model->type == POINTLIGHT ) continue;

		shader->setMat4("model", model->modelMatrix);
		shader->setMat4("view", camera.viewMatrix);
		shader->setMat4("projection", camera.getPerspectiveMatrix());
		// Render model
		model->mesh->Draw(shader);
	}
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void icarus3D::renderSceneLightingPass(Scene* scene, Shader* shader){

	// Change to default framebuffer
	glClearColor(0.78f, 0.78f, 0.78f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Draw skybox
		drawSkybox();
		// Draw grid
		drawGrid();
		// Set gBuffer textures uniforms
		shader->use();
		shader->setInt("gPosition", 0);
		shader->setInt("gNormal", 1);
		shader->setInt("gAlbedoSpec", 2);
		shader->setVec3("viewPos", camera.position);
		shader->setInt("ssao", 3);
		// Bind textures into GPU
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
		// Set lighting uniforms config
		setLightingUniforms(scene, shader);
		//Binds the vertex array to be drawn
		glBindVertexArray(VAO);
		// Renders the triangle geometry
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void icarus3D::renderStereoscopicViews() {
	if (currentScene == -1 || scene[currentScene]->models.size() == 0 || !stereoBool) return;

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Instance current scene
	Scene* scene = this->scene[currentScene];
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawGrid();
		drawSkybox();
		// Left Eye render
		glColorMask(true, false, false, false);
		// Iterate over scene models
		for (auto& model : scene->models) {

			// Use a single shader per model
			model->shader->use();

			if (model->type == MODEL) {
				// Set Directional light shader uniforms
				model->shader->setVec3("dirlight.direction", light->properties.direction);
				model->shader->setVec3("dirlight.color.ambient", light->properties.color.ambient);
				model->shader->setVec3("dirlight.color.diffuse", light->properties.color.diffuse);
				model->shader->setVec3("dirlight.color.specular", light->properties.color.specular);
				model->shader->setBool("dirlight.lightSwitch", light->lightSwitch);
				// Set Point lights shader uniforms
				model->shader->setInt("numOfPointLight", scene->pointlight_index.size());
				for (ICuint i = 0; i < scene->pointlight_index.size(); i++) {
					PointLight* pointlight = (PointLight*)scene->models[scene->pointlight_index[i]];
					std::string index = std::to_string(i);
					// Set pointlight position
					model->shader->setVec3("pointlight[" + index + "].position", pointlight->position);
					// Set pointlight color
					model->shader->setVec3("pointlight[" + index + "].color.ambient", pointlight->properties.color.ambient);
					model->shader->setVec3("pointlight[" + index + "].color.diffuse", pointlight->properties.color.diffuse);
					model->shader->setVec3("pointlight[" + index + "].color.specular", pointlight->properties.color.specular);
					// Set pointlight attenuationF
					model->shader->setFloat("pointlight[" + index + "].attenuation.constant", pointlight->properties.attenuation.constant);
					model->shader->setFloat("pointlight[" + index + "].attenuation.linear", pointlight->properties.attenuation.linear);
					model->shader->setFloat("pointlight[" + index + "].attenuation.quadratic", pointlight->properties.attenuation.quadratic);
					// Set pointlight switch bool
					model->shader->setBool("pointlight[" + index + "].lightSwitch", pointlight->lightSwitch);
					// Set directional light switch bool
				}
			}
			else if (model->type == POINTLIGHT) {
				PointLight* pointlight = (PointLight*)model;
				model->shader->setVec3("ambient", pointlight->properties.color.ambient);
				model->shader->setVec3("diffuse", pointlight->properties.color.diffuse);
				model->shader->setVec3("specular", pointlight->properties.color.specular);
				model->shader->setBool("lightSwitch", pointlight->lightSwitch);
			}

			// Set model shader uniforms
			model->shader->setVec3("viewPos", stereoscopic.left_eye->position);
			model->shader->setMat4("model", model->modelMatrix);
			model->shader->setMat4("view", stereoscopic.left_eye->viewMatrix);
			model->shader->setMat4("projection", stereoscopic.left_eye->perspectiveMatrix);
			model->shader->setVec4("colorFilter", stereoscopic.le_color);
			model->shader->setBool("anaglyph", true);

			// Render model
			model->mesh->Draw(model->shader);

		}

		glBindVertexArray(0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glColorMask(false, true, true, false);
		// Right Eye render
		// Iterate over scene models
		for (auto& model : scene->models) {

			// Use a single shader per model
			model->shader->use();

			if (model->type == MODEL) {
				// Set Directional light shader uniforms
				model->shader->setVec3("dirlight.direction", light->properties.direction);
				model->shader->setVec3("dirlight.color.ambient", light->properties.color.ambient);
				model->shader->setVec3("dirlight.color.diffuse", light->properties.color.diffuse);
				model->shader->setVec3("dirlight.color.specular", light->properties.color.specular);
				model->shader->setBool("dirlight.lightSwitch", light->lightSwitch);
				// Set Point lights shader uniforms
				model->shader->setInt("numOfPointLight", scene->pointlight_index.size());
				for (ICuint i = 0; i < scene->pointlight_index.size(); i++) {
					PointLight* pointlight = (PointLight*)scene->models[scene->pointlight_index[i]];
					std::string index = std::to_string(i);
					// Set pointlight position
					model->shader->setVec3("pointlight[" + index + "].position", pointlight->position);
					// Set pointlight color
					model->shader->setVec3("pointlight[" + index + "].color.ambient", pointlight->properties.color.ambient);
					model->shader->setVec3("pointlight[" + index + "].color.diffuse", pointlight->properties.color.diffuse);
					model->shader->setVec3("pointlight[" + index + "].color.specular", pointlight->properties.color.specular);
					// Set pointlight attenuationF
					model->shader->setFloat("pointlight[" + index + "].attenuation.constant", pointlight->properties.attenuation.constant);
					model->shader->setFloat("pointlight[" + index + "].attenuation.linear", pointlight->properties.attenuation.linear);
					model->shader->setFloat("pointlight[" + index + "].attenuation.quadratic", pointlight->properties.attenuation.quadratic);
					// Set pointlight switch bool
					model->shader->setBool("pointlight[" + index + "].lightSwitch", pointlight->lightSwitch);
					// Set directional light switch bool
				}
			}
			else if (model->type == POINTLIGHT) {
				PointLight* pointlight = (PointLight*)model;
				model->shader->setVec3("ambient", pointlight->properties.color.ambient);
				model->shader->setVec3("diffuse", pointlight->properties.color.diffuse);
				model->shader->setVec3("specular", pointlight->properties.color.specular);
				model->shader->setBool("lightSwitch", pointlight->lightSwitch);
			}

			// Set model shader uniforms
			model->shader->setVec3("viewPos", stereoscopic.right_eye->position);
			model->shader->setMat4("model", model->modelMatrix);
			model->shader->setMat4("view", stereoscopic.right_eye->viewMatrix);
			model->shader->setMat4("projection", stereoscopic.right_eye->perspectiveMatrix);
			model->shader->setVec4("colorFilter", stereoscopic.re_color);
			model->shader->setBool("anaglyph", true);

			// Render model
			model->mesh->Draw(model->shader);

		}

		glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glColorMask(true, true, true, true);
}

void icarus3D::setLightingUniforms(Scene* scene, Shader* shader) {
	// Set Directional light shader uniforms
	setDirectionalLightUniform(scene, shader);
	// Set pointlight shader uniforms
	setPointlightsUniform(scene, shader);
}

void icarus3D::setDirectionalLightUniform(Scene* scene, Shader* shader) {
	shader->setVec3("dirlight.direction", light->properties.direction);
	shader->setVec3("dirlight.color.ambient", light->properties.color.ambient);
	shader->setVec3("dirlight.color.diffuse", light->properties.color.diffuse);
	shader->setVec3("dirlight.color.specular", light->properties.color.specular);
	shader->setBool("dirlight.lightSwitch", light->lightSwitch);
	shader->setInt("numOfPointLight", scene->pointlight_index.size());
}

void icarus3D::setPointlightsUniform(Scene* scene, Shader* shader) {
	// If we are in SSAO mode transform all pointlight position into view space
	if (ssaoBool) {
		for (ICuint i = 0; i < scene->pointlight_index.size(); i++) {
			PointLight* pointlight = (PointLight*)scene->models[scene->pointlight_index[i]];
			std::string index = std::to_string(i);
			// Set pointlight position
			shader->setVec3("pointlight[" + index + "].position", glm::vec3(camera.viewMatrix * glm::vec4(pointlight->position,1.0f)));
			// Set pointlight color
			shader->setVec3("pointlight[" + index + "].color.ambient", pointlight->properties.color.ambient);
			shader->setVec3("pointlight[" + index + "].color.diffuse", pointlight->properties.color.diffuse);
			shader->setVec3("pointlight[" + index + "].color.specular", pointlight->properties.color.specular);
			// Set pointlight attenuationF
			shader->setFloat("pointlight[" + index + "].attenuation.constant", pointlight->properties.attenuation.constant);
			shader->setFloat("pointlight[" + index + "].attenuation.linear", pointlight->properties.attenuation.linear);
			shader->setFloat("pointlight[" + index + "].attenuation.quadratic", pointlight->properties.attenuation.quadratic);
			// Set pointlight switch bool
			shader->setBool("pointlight[" + index + "].lightSwitch", pointlight->lightSwitch);
		}
	}
	// Keep working in clipping space
	else {
		for (ICuint i = 0; i < scene->pointlight_index.size(); i++) {
			PointLight* pointlight = (PointLight*)scene->models[scene->pointlight_index[i]];
			std::string index = std::to_string(i);
			// Set pointlight position
			shader->setVec3("pointlight[" + index + "].position", pointlight->position);
			// Set pointlight color
			shader->setVec3("pointlight[" + index + "].color.ambient", pointlight->properties.color.ambient);
			shader->setVec3("pointlight[" + index + "].color.diffuse", pointlight->properties.color.diffuse);
			shader->setVec3("pointlight[" + index + "].color.specular", pointlight->properties.color.specular);
			// Set pointlight attenuationF
			shader->setFloat("pointlight[" + index + "].attenuation.constant", pointlight->properties.attenuation.constant);
			shader->setFloat("pointlight[" + index + "].attenuation.linear", pointlight->properties.attenuation.linear);
			shader->setFloat("pointlight[" + index + "].attenuation.quadratic", pointlight->properties.attenuation.quadratic);
			// Set pointlight switch bool
			shader->setBool("pointlight[" + index + "].lightSwitch", pointlight->lightSwitch);
		}
	}

}

void icarus3D::drawBoundingBox() {
	// Render Bounding box
	boundingBoxShader->use();
	boundingBoxShader->setMat4("model", scene[currentScene]->models[pickedIndex]->modelMatrix);
	boundingBoxShader->setMat4("view", camera.viewMatrix);
	boundingBoxShader->setMat4("projection", camera.getPerspectiveMatrix());

	// Draw Bounding Box
	scene[currentScene]->models[pickedIndex]->DrawBoundingBox();
}

void icarus3D::drawGrid() {
	
	gridShader->use();
	gridShader->setMat4("model", gridModelMatrix);
	gridShader->setMat4("view", camera.viewMatrix);
	gridShader->setMat4("projection", camera.getPerspectiveMatrix());

	glBindVertexArray(gridVAO);

	glDrawElements(GL_LINES, gridLength, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);

}

void icarus3D::drawSkybox() {
	// Draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->use();
	skyboxShader->setInt("skybox", 0);
	skyboxShader->setMat4("view", glm::mat4(glm::mat3(camera.viewMatrix)));
	skyboxShader->setMat4("projection", camera.perspectiveMatrix);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// set depth function back to default
	glDepthFunc(GL_LESS); 
}

void icarus3D::render() {

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		//FPS
		updateFrames();

		processKeyboardInput(window);

		// Render
		// Clear the colorbuffer
		glClearColor(0.78f, 0.78f, 0.78f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);



		// Render grid and skybox when a new scene is selected
		if (currentScene != -1) {
			drawSkybox();
			drawGrid();
		}

		// Render all (Deferred shading)
		deferredShading();

		// Stereoscopic view pass
		renderStereoscopicViews();
	
		// Render picked object bounding box
		renderBoundingBox();
		// Render light models
		renderPointlightModels();
		// Render particle system
		renderParticleSystem();
		// Draw interface
		ui.draw();
		// Check for collision
		checkCollision();
		// Swap the screen buffers
		glfwSwapBuffers(window);
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
	}
	
}

void icarus3D::deferredShading() {
	// If there's no scene picked then don't render anything
	if (currentScene == -1 || scene[currentScene]->models.size() == 0 || stereoBool) return;

	
		if (ssaoBool) {
			// First pass: Geometry pass into g-buffer textures
			renderSceneGeometryPass(scene[currentScene], ssaoGeometryShader);
			// SSAO pass and blur pass
			renderSSAO();
			// Lighting Pass (in view space)
			renderSceneLightingPass(scene[currentScene],ssaoLightingPassShader);
		}
		else {
			// First pass: Geometry pass into g-buffer textures
			renderSceneGeometryPass(scene[currentScene], geometryPassShader);
			// Render second pass (Lighting) -> into framebuffer's texture (which is fTexture)
			renderSceneLightingPass(scene[currentScene], lightingPassShader);
		}
		

	// Apply DoF technique (if it's turned ON)
	if (depthOfFieldBool) depthOfField();


	// Render final pass
	renderTexture(fTexture);


}

bool icarus3D::addModel(const string path, const string name) {
	string mtl_path = path;
	// Find mtl path
	size_t pos = 0;
	string token;
	pos = path.find(".");
	mtl_path = mtl_path.substr(0, pos);
	mtl_path += ".mtl";
	scene[currentScene]->addModel(path,mtl_path,name);
	return true;
}

bool icarus3D::addLight(const string name) {
	scene[currentScene]->addLight(name);
	return true;
}

bool icarus3D::createScene(string name) {
	Scene *newScene = new Scene();
	if (name == "")
		name = "Scene" + to_string(this->scene.size());
	newScene->name = name;
	scene.push_back(newScene);
	cout << "scene created successfully" << endl;
	// Choose, by default, freshly new scene
	instance->currentScene = instance->scene.size() - 1;
	instance->setPickedIndex(-1);
	return true;
}

bool icarus3D::loadScene(string path, string name) {

	Scene* newScene = new Scene();
	newScene->loadScene(path, name);
	scene.push_back(newScene);

	// Choose, by default, freshly new scene
	instance->currentScene = instance->scene.size() - 1;
	instance->setPickedIndex(-1);

	return true;
}

bool icarus3D::saveScene(string path) {

	scene[currentScene]->saveScene(path);
	return true;
}

void icarus3D::updateFrames() {

	lastTime = currentTime;
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;

	totalFrames++;

	if (totalTime > 1) {
		fps = totalFrames;
		totalFrames = 0;
		totalTime = 0;

		return;
	}

	totalTime += deltaTime;
}

bool icarus3D::checkCollision() {
	// if there's no scene return
	if (currentScene == -1) return false;

	Scene* scene = this->scene[currentScene];
	collisionBool = false;
	
	// Iterate over scene models
	for (int i = 0; i < scene->models.size(); i++) {

		glm::mat4 scale = scene->models[i]->scalingMatrix;

		glm::vec4 minScaled = glm::vec4(scene->models[i]->mesh->min.x,
										scene->models[i]->mesh->min.y,
										scene->models[i]->mesh->min.z,
										0);


		glm::vec4 maxScaled = glm::vec4(scene->models[i]->mesh->max.x,
										scene->models[i]->mesh->max.y,
										scene->models[i]->mesh->max.z,
										0);


		maxScaled = scale * maxScaled;
		minScaled = scale * minScaled;

		//cout << "MIN SCALED" << endl;
		//cout << minScaled.x << ", " << minScaled.y << ", " << minScaled.z << endl;

		if (camera.position.x + camera.nearPlane > scene->models[i]->position.x + minScaled.x
			&& camera.position.y + camera.nearPlane  > scene->models[i]->position.y + minScaled.y
			&& camera.position.z + camera.nearPlane  > scene->models[i]->position.z + minScaled.z
			&& camera.position.x - camera.nearPlane  < scene->models[i]->position.x + maxScaled.x
			&& camera.position.y - camera.nearPlane  < scene->models[i]->position.y + maxScaled.y
			&& camera.position.z - camera.nearPlane  < scene->models[i]->position.z + maxScaled.z){
			collisionBool = true;
			return true;
		}
	//	
	//	glm::vec4 minBB = glm::vec4(    scene->models[i]->mesh->min.x,
	//									scene->models[i]->mesh->min.y,
	//									scene->models[i]->mesh->min.z,
	//									0);



	//	glm::vec4 maxBB = glm::vec4(    scene->models[i]->mesh->max.x,
	//									scene->models[i]->mesh->max.y,
	//									scene->models[i]->mesh->max.z,
	//									0);
	//	
	//	minBB = scene->models[i]->modelMatrix * minBB;
	//	maxBB = scene->models[i]->modelMatrix * maxBB;

	//	if (   camera.position.x + camera.nearPlane >  minBB.x
	//		&& camera.position.y + camera.nearPlane >  minBB.y
	//		&& camera.position.z + camera.nearPlane >  minBB.z
	//		&& camera.position.x - camera.nearPlane <  maxBB.x
	//		&& camera.position.y - camera.nearPlane <  maxBB.y
	//		&& camera.position.z - camera.nearPlane <  maxBB.z){
	//		collisionBool = true;
	//		return true;
	//	}
	}
	return false;
}

void icarus3D::pick() {

	if (scene[currentScene]->models.size() == 0)
		return;

	double cursorX, cursorY;

	// Clear the screen in white
	glClearColor(0.78f, 0.78f, 0.78f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get Cursor position
	glfwGetCursorPos(window, &cursorX, &cursorY);
	cursorX = float(cursorX);
	cursorY = windowHeight - float(cursorY);
	// Set shader
	pickingShader->use();
	// Set projection matrix
	pickingShader->setMat4("projection", camera.getPerspectiveMatrix());
	pickingShader->setMat4("view", camera.viewMatrix);
	
	// Render all scene
		// Iterate over scene models
		for (auto& model : scene[currentScene]->models) {
			// Set model matrix
			pickingShader->setMat4("model", model->modelMatrix);

			// Set model picking color
			pickingShader->setVec3("pickingColor", model->pickingColor);

			// Render model
			model->mesh->Draw(model->shader);
		}

		// Wait until all the pending drawing commands are really done. SLOW
		glFlush();
		glFinish();

		// Read the pixel where click happened. SLOW 
		vec4 readPixel;
		glReadPixels((int)cursorX, (int)cursorY, 1, 1, GL_RGBA, GL_FLOAT, &readPixel);
		// Round color floats to be able to compare
		readPixel.r = roundf(readPixel.r * 100) / 100;

		for (auto &model : scene[currentScene]->models) {
			if (abs(readPixel.r - model->pickingColor.r) < 0.001 && readPixel.r != 0.0f) {
				pickedIndex = &model - &scene[currentScene]->models[0];
				printf("picked[%i] - name: %s\n", pickedIndex, model->name.c_str());

			}
		}
		// Uncomment these lines to see the picking shader in effect
		//glfwSwapBuffers(window);
}

unsigned int icarus3D::loadTexture(const char* path)
{
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Loads the texture
	int textureWidth, textureHeight, numberOfChannels;
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);
	// Loads the texture file data
	unsigned char* data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
	if (data)
	{
		// Gets the texture channel format
		GLenum format;
		switch (numberOfChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		// Binds the texture
		glBindTexture(GL_TEXTURE_2D, id);
		// Creates the texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
		// Creates the texture mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);

	return id;
}

void icarus3D::depthOfField() {

	// Ping-pong textures to re-use framebuffer
	duplicateTexture(fTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use the shader
		deferredDepthShader->use();
		deferredDepthShader->setInt("depthMap", 0);
		deferredDepthShader->setInt("image", 1);
		deferredDepthShader->setInt("kernel7", 2);
		deferredDepthShader->setInt("kernel11", 3);

		deferredDepthShader->setFloat("DOFThreshold", scene[currentScene]->DOFThreshold);
		deferredDepthShader->setFloat("near_plane", camera.nearPlane);
		deferredDepthShader->setFloat("far_plane", camera.farPlane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingPongTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_1D, kernel7);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_1D, kernel11);
		//Binds the vertex array to be drawn
		glBindVertexArray(VAO);
		// Renders the triangle geometry
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void icarus3D::renderPointlightModels() {
	// if there's no scene created return
	if (currentScene == -1) return;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for (auto& model : scene[currentScene]->models) {
		model->shader->use();

		if (model->type == POINTLIGHT) {
			PointLight* pointlight = (PointLight*)model;
			model->shader->setVec3("ambient", pointlight->properties.color.ambient);
			model->shader->setVec3("diffuse", pointlight->properties.color.diffuse);
			model->shader->setVec3("specular", pointlight->properties.color.specular);
			model->shader->setBool("lightSwitch", pointlight->lightSwitch);

			// Set model shader uniforms
			model->shader->setVec3("viewPos", camera.position);
			model->shader->setMat4("model", model->modelMatrix);
			model->shader->setMat4("view", camera.viewMatrix);
			model->shader->setMat4("projection", camera.getPerspectiveMatrix());

			// Render model
			model->mesh->Draw(model->shader);
		}
	}
}

void icarus3D::renderBoundingBox() {
	if (currentScene == -1) return;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for (auto& model : scene[currentScene]->models) {
		if (&model - &scene[currentScene]->models[0] == pickedIndex)
			drawBoundingBox();
	}
}

void icarus3D::renderSSAO() {

	// Use G-buffer to render SSAO Texture
	glClearColor(0.78f, 0.78f, 0.78f, 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
		ssaoShader->use();
		// Send kernel + rotation
		for (unsigned int i = 0; i < ssao.kernelSize; ++i)
			ssaoShader->setVec3("samples[" + std::to_string(i) + "]", ssao.kernel[i]);
		ssaoShader->setMat4("projection", camera.perspectiveMatrix);
		ssaoShader->setInt("gPosition", 0);
		ssaoShader->setInt("gNormal", 1);
		ssaoShader->setInt("noiseTexture", 2);
		ssaoShader->setFloat("radius",ssao.radius);
		ssaoShader->setFloat("bias",ssao.bias);
		ssaoShader->setInt("kernelSize",ssao.kernelSize);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		//Binds the vertex array to be drawn
		glBindVertexArray(VAO);
		// Renders the triangle geometry
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// BLUR SSAO Texture
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderSSAOBlur->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glBindVertexArray(VAO);
	// Renders the triangle geometry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void icarus3D::renderTexture(GLuint texture) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0); {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		debugTextureShader->use();
		debugTextureShader->setInt("texImage", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		//Binds the vertex array to be drawn
		glBindVertexArray(VAO);
		// Renders the triangle geometry
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void icarus3D::buildQuad() {

	// Quad for debug purposes:
	float quadVertices[] = {
		// positions        // Color   		   // texture Coords
		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 0.5f, 0.5f, 0.75f, 1.0f, 0.0f,
	};
	// Setup plane VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	// Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

}

bool icarus3D::setFrameBuffer(GLuint &framebuffer, GLuint& texture) {

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// The texture we're going to render to
	glGenTextures(1, &texture);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// Give an empty image to OpenGL (Which is done with last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Depth buffer
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	// Frame buffer configuration
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

	// Set list of draw buffers
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::Framebuffer configuration went wrong" << std::endl;
		return false;
	}
	return true;
}

bool icarus3D::setSSAOFramebuffer() {
	// Generate SSAO FBO 
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	// Generate SSAO color buffer which will hold occlusion value for each generated fragment
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	// Only red channel because output will be in grayscale
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

	// Generate SSAO FBO BLUR
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

	return true;
}

bool icarus3D::setGeometryBuffer() {
	// Bind geometry buffer
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// Mtl color contribution buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	GLenum attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// Depth buffer
	glGenRenderbuffers(1, &gDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepth);

	// Depth buffer into texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 0.78f, 0.78f, 0.78f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	// Check if framebuffer is succesfully completed
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "ERROR::Geometry buffer initialization has failed" << endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return true;
}

bool icarus3D::initKernel() {

	//prepare for blur
	vector<int> kernelData = vector<int>(49, 1);

	glGenTextures(1, &kernel7);
	glBindTexture(GL_TEXTURE_1D, kernel7);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, 49, 0, GL_RED_INTEGER, GL_INT, kernelData.data());

	glBindTexture(GL_TEXTURE_1D, 0);

	//prepare for blur
	kernelData = vector<int>(121, 1);

	glGenTextures(1, &kernel11);
	glBindTexture(GL_TEXTURE_1D, kernel11);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32I, 121, 0, GL_RED_INTEGER, GL_INT, kernelData.data());

	glBindTexture(GL_TEXTURE_1D, 0);


	return true;

}

bool icarus3D::initSSAO() {

	// Compute SSAO kernel
	computeSSAOKernel();

	// 4x4 array of random rotation vectors oriented around tangent-space surface normal
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++){
		// rotate around z-axis (in tangent space)
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			// Since sample kernel is oriented along positive z direction in tangent space leave component z at 0.0
			// So we rotate around z-axis
			0.0f); 
		ssaoNoise.push_back(noise);
	}

	// Generate noise texture
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return true;
}

void icarus3D::computeSSAOKernel() {
	ssao.kernel.clear();
	// Compute SSAO kernel
	std::default_random_engine generator;
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
	for (unsigned int i = 0; i < ssao.kernelSize; i++) {
		glm::vec3 sample(
			// Vary x and y direction between -1.0 and 1.0
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			// Vary z between 0.0 - 1.0f 
			// If we vary between -1.0 and 1.0 we'd have a sphere sample instead of hemisphere
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / ssao.kernelSize;

		// scale samples so they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssao.kernel.push_back(sample);
	}
}

void icarus3D::duplicateTexture(GLuint target) {
	glBindFramebuffer(GL_FRAMEBUFFER, pingPongFramebuffer); {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		debugTextureShader->use();
		debugTextureShader->setInt("texImage", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, target);
		//Binds the vertex array to be drawn
		glBindVertexArray(VAO);
		// Renders the triangle geometry
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void icarus3D::renderParticleSystem() {
	if (!particlesystemBool) return;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	particleSystem->update(deltaTime);
	particleSystem->draw(particleSystemShader, particleSystemTexture, camera.viewMatrix, camera.perspectiveMatrix);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

bool icarus3D::setParticleSystemTexture(char* path) {
	char dir_path[100] = "assets/textures/";
	strcat(dir_path, path);
	particleSystemTexture = loadTexture(dir_path);
	return true;
}

