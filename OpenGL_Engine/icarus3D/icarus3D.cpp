#include "icarus3D.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Static Variables
ICuint icarus3D::windowWidth = 800;
ICuint icarus3D::windowHeight = 600;

// Global static pointer used to ensure a single instance of the class.
icarus3D* icarus3D::instance = NULL;
Camera icarus3D::camera(windowWidth, windowHeight);
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
	if (!initWindow() || !initGlad() || !setFrameBuffer(dsTexture)) {
		std::cout << "ERROR::Couldn't initialize window or GLAD components" << std::endl;
		return;
	}
	// Initialize OpenGL context
	initGL();
	// Initialize user interface context
	ui.init(window);

	// Init Icarus3D variables
	pickingShader = new Shader("icarus3D/shaders/picking.vert", "icarus3D/shaders/picking.frag");
	boundingBoxShader = new Shader("icarus3D/shaders/bounding_box.vert", "icarus3D/shaders/bounding_box.frag");
	deferredShader = new Shader("icarus3D/shaders/deferred.vert", "icarus3D/shaders/deferred.frag");
	whiteTexture = loadTexture("assets/textures/white_bg.png");
	blackTexture = loadTexture("assets/textures/black_bg.png");
	buildDeferredPlane();
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

	camera.resize(windowWidth, windowHeight);
	instance->setFrameBuffer(instance->dsTexture);
}

void icarus3D::renderScene(Scene *scene) {
	// Iterate over scene models
	for (auto& model : scene->models) {
		
		// Use a single shader per model
		model->shader->use();
	
		// Set model shader configuration

		model->shader->setVec3("light.direction", light->properties.direction);
		model->shader->setVec3("light.color.ambient", light->properties.color.ambient);
		model->shader->setVec3("light.color.diffuse", light->properties.color.diffuse);
		model->shader->setVec3("light.color.specular", light->properties.color.specular);
		model->shader->setVec3("viewPos", camera.position);
		model->shader->setFloat("shininess", 32.0);
		model->shader->setMat4("model", model->modelMatrix);
		model->shader->setMat4("view", camera.getWorldToViewMatrix());
		model->shader->setMat4("projection", camera.getPerspectiveMatrix());
	
		// Render model
		model->mesh->Draw(model->shader);
		if (&model - &scene->models[0] == pickedIndex)
			drawBoundingBox();
	}
}

void icarus3D::drawBoundingBox() {
	// Render Bounding box
	boundingBoxShader->use();
	boundingBoxShader->setMat4("model", scene[currentScene]->models[pickedIndex]->modelMatrix);
	boundingBoxShader->setMat4("view", camera.getWorldToViewMatrix());
	boundingBoxShader->setMat4("projection", camera.getPerspectiveMatrix());

	// Draw Bounding Box
	scene[currentScene]->models[pickedIndex]->DrawBoundingBox();
}

void icarus3D::render() {

	// Game loop
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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// If there is any instanced scene, then render it
		if (currentScene != -1) {

			if (checkCollision(scene[currentScene])) {
				cout << "COLLISION!" << endl;
			}

			//Deferred shading
			renderToTexture();

			forwardRendering();
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
	pickingShader->setMat4("view", camera.getWorldToViewMatrix());
	
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

void icarus3D::renderToTexture() {

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// Clears the color and depth buffers from the frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render scene
	renderScene(scene[currentScene]);

	glBindVertexArray(0);

}

void icarus3D::forwardRendering() {
	// Forward rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Use the shader
	deferredShader->use();
	deferredShader->setInt("image", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dsTexture);
	//Binds the vertex array to be drawn
	glBindVertexArray(VAO);
	// Renders the triangle geometry
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void icarus3D::buildDeferredPlane() {

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

bool icarus3D::setFrameBuffer(GLuint& texture) {

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
