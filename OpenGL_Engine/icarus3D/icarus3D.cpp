#include "icarus3D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

// Global static pointer used to ensure a single instance of the class.
icarus3D* icarus3D::instance = NULL;
Camera icarus3D::camera;

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
	// Begin render loop

	render();

	// Terminate interface instance
	ui.terminate();
	// Terminate GLFW, clearing any resources allocated by GLFW.
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

	return true;
}

void icarus3D::onMouseMotion(ICwindow* window, double xpos, double ypos)
{
	glm::vec2 mousePosition(xpos, ypos);
	camera.mouseUpdate(mousePosition);
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

void icarus3D::resize(ICwindow* window, int width, int height){
	windowWidth = width;
	windowHeight = height;
	// Sets the OpenGL viewport size and position
	glViewport(0, 0, windowWidth, windowHeight);
}

void icarus3D::renderScene(std::vector<Model>& scene) {
	// Iterate over scene models
	for (auto model = scene.begin(); model != scene.end();model++) {
		
		// Use a single shader per model
		model->shader->use();
		// MVP matrix per model part
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 1.0f, 100.0f);
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::vec3 modelPosition = model->position;
		modelMatrix = glm::translate(modelMatrix, modelPosition);
		glm::mat4 viewMatrix = camera.getWorldToViewMatrix();
			
		// Set model shader configuration
		model->shader->setMat4("model", modelMatrix);
		model->shader->setMat4("view", viewMatrix);
		model->shader->setMat4("projection", projectionMatrix);
		
		// Render model
		model->mesh->Draw();
	}
}

void icarus3D::render() {
	// Game loop

	while (!glfwWindowShouldClose(window))
	{
		processKeyboardInput(window);

		// Render
		// Clear the colorbuffer
		glClearColor(0.78f, 0.78f, 0.78f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Render scene
		renderScene(models);
		
		// Draw interface
		ui.draw();
		// Swap the screen buffers
		glfwSwapBuffers(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
	}
	
}

bool icarus3D::addModel(std::vector<Model>& scene) {
	Model newModel;
	newModel.setShader("icarus3D/shaders/basic.vert", "icarus3D/shaders/basic.frag");
	newModel.loadMesh("assets/models/Knight.obj");
	scene.push_back(newModel);

	return true;
}

void icarus3D::processKeyboardInput(GLFWwindow* window)
{
	// Checks if the escape key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// Tells glfw to close the window as soon as possible
		glfwSetWindowShouldClose(window, true);

	lastTime = currentTime;
	currentTime = glfwGetTime();

	float deltaTime = currentTime - lastTime;

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
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.moveUp(deltaTime);
	}
	// Move Down
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.moveDown(deltaTime);
	}

}