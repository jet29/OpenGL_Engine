#include "icarus3D.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Global static pointer used to ensure a single instance of the class.
icarus3D* icarus3D::instance = NULL;
Camera icarus3D::camera;
bool icarus3D::cameraMode = false;

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
	if (!initWindow() || !initGlad()) {
		std::cout << "ERROR::Couldn't initialize window or GLAD components" << std::endl;
		return;
	}
	// Initialize OpenGL context
	initGL();
	// Initialize user interface context
	ui.init(window);
	// Load picking shader
	pickingShader = new Shader("icarus3d/shaders/picking.vert", "icarus3d/shaders/picking.frag");
	// Create Mandatory Dir Light
	light = new DirectionalLight();
	// Begin render loop
	render();
	// Terminate interface instance
	ui.terminate();
	// Terminate GLFW, clearing any resources allocated by GLFW.s
	glfwTerminate();
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
	//glfwSetFramebufferSizeCallback(window, resize);
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
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

void icarus3D::processKeyboardInput(ICwindow* window)
{
	// Checks if the escape key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// Tells glfw to close the window as soon as possible
		glfwSetWindowShouldClose(window, true);

	float deltaTime = currentTime - lastTime;
	if (cameraMode) {
		// Move Forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.moveForward(deltaTime);
		}
		// Move Backward
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.moveBackward(deltaTime);
		}
		// Move right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.moveRight(deltaTime);
		}
		// Move left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.moveLeft(deltaTime);
		}

		// Move Up
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.moveUp(deltaTime);
		}
		// Move Down
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.moveDown(deltaTime);
		}
	}


}

void icarus3D::onKeyPress(ICwindow* window, int key, int scancode, int action, int mods) {
	// Actions when camera mode is disabled
	if (action == GLFW_PRESS && !cameraMode) {
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
			for (auto &model : instance->scene[instance->currentScene]->models) {
				model->setShader(model->shaderPath[0], model->shaderPath[1]);
			}
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
}

void icarus3D::onMouseMotion(ICwindow* window, double xpos, double ypos)
{
	if (cameraMode){
		glfwSetCursorPos(window, instance->windowWidth / 2.0, instance->windowHeight / 2.0);
		double xoffset = ((instance->windowWidth / 2.0) - xpos) * camera.mouseSpeed * instance->deltaTime;
		double yoffset = ((instance->windowHeight / 2.0) - ypos) * camera.mouseSpeed * instance->deltaTime;

		camera.mouseUpdate(glm::vec2(xoffset, yoffset));
	}
}

void icarus3D::onMouseButton(ICwindow* window, int button, int action, int mods){
	//auto a = action == GLFW_PRESS ? GLFW_PRESS : GLFW_RELEASE;
	//auto b = GLFW_MOUSE_BUTTON_LEFT;

	if (action == GLFW_PRESS && instance->currentScene != -1)
		instance->pick();
}

void icarus3D::resize(ICwindow* window, int width, int height){
	windowWidth = width;
	windowHeight = height;
	// Sets the OpenGL viewport size and position
	glViewport(0, 0, windowWidth, windowHeight);
}

void icarus3D::renderScene(Scene *scene) {
	// Iterate over scene models
	for (auto& model : scene->models) {
		
		// Use a single shader per model
		model->shader->use();
		// MVP matrix per model part
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 1.0f, 100.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::vec3 modelPosition = model->position;
		modelMatrix = glm::translate(modelMatrix, modelPosition);
		modelMatrix *= model->rotationMatrix;
		glm::mat4 viewMatrix = camera.getWorldToViewMatrix();
			
		// Set model shader configuration

		model->shader->setVec3("light.direction", light->properties.direction);
		model->shader->setVec3("light.color.ambient", light->properties.color.ambient);
		model->shader->setVec3("light.color.diffuse", light->properties.color.diffuse);
		model->shader->setVec3("light.color.specular", light->properties.color.specular);
		model->shader->setVec3("viewPos", camera.position);
		model->shader->setFloat("shininess", 32.0);
		model->shader->setMat4("model", modelMatrix);
		model->shader->setMat4("view", viewMatrix);
		model->shader->setMat4("projection", projectionMatrix);
	
		// Render model
		model->mesh->Draw(model->shader);
		if (&model - &scene->models[0] == pickedIndex)
			drawBoundingBox();
	}
}

void icarus3D::drawBoundingBox() {
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 1.0f, 1000.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix *= scene[currentScene]->models[pickedIndex]->rotationMatrix;
	glm::vec3 modelPosition = scene[currentScene]->models[pickedIndex]->position;
	modelMatrix = glm::translate(modelMatrix, modelPosition);
	glm::mat4 viewMatrix = camera.getWorldToViewMatrix();
	// Render Bounding box
	boundingBox->use();
	boundingBox->setMat4("model", modelMatrix);
	boundingBox->setMat4("view", viewMatrix);
	boundingBox->setMat4("projection", projectionMatrix);
	scene[currentScene]->models[pickedIndex]->DrawBoundingBox(projectionMatrix, viewMatrix, modelMatrix);
}

void icarus3D::render() {
	// Game loop
	whiteTexture = loadTexture("assets/textures/white_bg.png");
	blackTexture = loadTexture("assets/textures/black_bg.png");
	cout << "init: " + to_string(whiteTexture) << endl;
	boundingBox = new Shader("icarus3D/shaders/bounding_box.vert", "icarus3D/shaders/bounding_box.frag");
	while (!glfwWindowShouldClose(window))
	{
		//FPS
		updateFrames();
		//cout << "FPS: " << getFPS()<< endl;

		processKeyboardInput(window);

		// Render
		// Clear the colorbuffer
		glClearColor(0.78f, 0.78f, 0.78f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// If there is any instanced scene, then render it
		if (currentScene != -1) {

			if (checkCollision(scene[currentScene])) {
				cout << "COLLISION!" << endl;
			}
			// Render scene
			renderScene(scene[currentScene]);
		}
		
		// Draw interface
		ui.draw();
		// Swap the screen buffers
		glfwSwapBuffers(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
	}
	
}

bool icarus3D::addModel(const string path) {
	string mtl_path = path;
	// Find mtl path
	size_t pos = 0;
	string token;
	pos = path.find(".");
	mtl_path = mtl_path.substr(0, pos);
	mtl_path += ".mtl";
	scene[currentScene]->addModel(path,mtl_path);

	return true;
}

bool icarus3D::createScene() {
	
	Scene *newScene = new Scene();
	scene.push_back(newScene);
	cout << "scene created successfully" << endl;
	// Choose, by default, freshly new scene
	instance->currentScene = instance->scene.size() - 1;
	instance->setPickedIndex(-1);
	return true;
}

bool icarus3D::loadScene(string path) {

	Scene* newScene = new Scene();
	newScene->loadScene(path);
	scene.push_back(newScene);

	return true;
}

bool icarus3D::saveScene() {
	if (!currentScene == -1) {
		cout << "there is no scene loaded " << endl;
		return false;
	}

	scene[currentScene]->saveScene();
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

bool icarus3D::checkCollision(Scene *scene) {


	//cout << "camera pos: " << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << endl;
	
	// Iterate over scene models
	for (int i = 0; i < scene->models.size(); i++) {
		//cout << "min model: " << model->mesh->min.x << ", " << model->mesh->min.y << ", " << model->mesh->min.z << endl;
		//cout << "max model: " << model->mesh->max.x << ", " << model->mesh->max.y << ", " << model->mesh->max.z << endl;

		if (   camera.position.x > scene->models[i]->position.x + scene->models[i]->mesh->min.x
			&& camera.position.y > scene->models[i]->position.y + scene->models[i]->mesh->min.y
			&& camera.position.z > scene->models[i]->position.z + scene->models[i]->mesh->min.z
			&& camera.position.x < scene->models[i]->position.x + scene->models[i]->mesh->max.x
			&& camera.position.y < scene->models[i]->position.y + scene->models[i]->mesh->max.y
			&& camera.position.z < scene->models[i]->position.z + scene->models[i]->mesh->max.z){
			return true;
		}
	}

	return false;
}

void icarus3D::pick() {

	if (scene[currentScene]->models.size() == 0)
		return;

	double cursorX, cursorY;

	// Clear the screen in white
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Get Cursor position
	glfwGetCursorPos(window, &cursorX, &cursorY);
	cursorX = float(cursorX);
	cursorY = windowHeight - float(cursorY);
	// Set shader
	pickingShader->use();
	// Set projection matrix
	pickingShader->setMat4("projection", glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 1.0f, 100.0f));
	pickingShader->setMat4("view", camera.getWorldToViewMatrix());
	
	// Render all scene
		// Iterate over scene models
		for (auto& model : scene[currentScene]->models) {
			// Compute Model matrix
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix *= model->rotationMatrix;
			glm::vec3 modelPosition = model->position;
			modelMatrix = glm::translate(modelMatrix, modelPosition);

			// Set model matrix
			pickingShader->setMat4("model", modelMatrix);

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
