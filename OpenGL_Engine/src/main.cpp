#include <iostream>

#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


using namespace std;

GLFWwindow* window;

// Window current width
GLuint windowWidth = 800;
// Window current height
GLuint windowHeight = 600;

// Texture
GLuint texture;
int textureWidth, textureHeight, numberOfChannels;

// Delta Time
float currentTime = 0;
float lastTime = 0;

// Camera starting position
glm::vec3 position = glm::vec3(0, 0, 1);

// Index (GPU) of the geometry buffer
unsigned int VBO;
// Index (GPU) vertex array object
unsigned int VAO;

void buildGeometry() {
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

void resize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	// Sets the OpenGL viewport size and position
	glViewport(0, 0, windowWidth, windowHeight);
}

bool initGlad() {
	// Initialize glad
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// If something went wrong during the glad initialization
	if (!status)
	{
		std::cout << status << std::endl;
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

unsigned int loadTexture(const char* path)
{
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);

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

bool initWindow() {
	// Initialize glfw
	glfwInit();
	// Sets the Opegl context to Opengl 3.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates the window
	window = glfwCreateWindow(windowWidth, windowHeight, "Image Editor window", NULL, NULL);

	// The window couldn't be created
	if (!window)
	{
		std::cout << "Failed to create the glfw window" << std::endl;
		glfwTerminate(); // Stops the glfw program
		return false;
	}

	// Creates the glfwContext, this has to be made before calling initGlad()
	glfwMakeContextCurrent(window);

	// Window resize callback
	glfwSetFramebufferSizeCallback(window, resize);

	return true;
}

void initGL() {
	// Enables the z-buffer test
	glEnable(GL_DEPTH_TEST);
	// Sets the ViewPort
	glViewport(0, 0, windowWidth, windowHeight);
	// Sets the clear color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

bool init() {
	// Initialize window and glad components
	if (!initWindow() || !initGlad())
		return false;
	// Initialize OpenGL context
	initGL();
	// Initialize geometry for GPU
	buildGeometry();

	// Load texture
	texture = loadTexture("assets/textures/texture.jpg");

	cout << "texture size" << endl;
	cout << textureWidth << ", " << textureHeight << endl;

	return true;
}

void processKeyboardInput(GLFWwindow* window)
{
	// Checks if the escape key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// Tells glfw to close the window as soon as possible
		glfwSetWindowShouldClose(window, true);

	lastTime = currentTime;
	currentTime = glfwGetTime();

	float deltaTime = currentTime - lastTime;

	// Move Up
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += glm::vec3(0, 1, 0) * deltaTime * 4.0f;
	}
	// Move Down
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position += glm::vec3(0, -1, 0) * deltaTime * 4.0f;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += glm::vec3(1, 0, 0) * deltaTime * 4.0f;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position += glm::vec3(-1, 0, 0) * deltaTime * 4.0f;
	}

	// Strafe Forward
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		position += glm::vec3(0, 0, -1) * deltaTime * 4.0f;
	}
	// Strafe Back
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		position += glm::vec3(0, 0, 1) * deltaTime * 4.0f;
	}


}



// The MAIN function, from here we start the application and run the game loop
int main()
{
	init();

	Shader* shader = new Shader("src/shaders/basic.vert", "src/shaders/basic.frag");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		processKeyboardInput(window);

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//SETUP MVP
		glm::vec3 direction = glm::vec3(0, 0, -1);
		glm::vec3 up = glm::vec3(0, 1, 0);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, .5f, 1000.0f);
		glm::mat4 view = glm::lookAt(
			position, // Camera is at (4,3,3), in world space
			position + direction, // and looks at the origin
			up  // Head is up (set to 0,-1,0 to look upside-down) 
		);

		//difference of heigth for width
		float aspectRatio = (float)textureHeight / (float)textureWidth;

		//cout << "aspect ratio: " << aspectRatio << endl;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1, aspectRatio, 1));

		shader->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		shader->setInt("image", 0);

		shader->setMat4("model", model);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		// Binds the vertex array to be drawn
		glBindVertexArray(VAO);
		// Render triangle's geometry
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	delete shader;

	return EXIT_SUCCESS;
}