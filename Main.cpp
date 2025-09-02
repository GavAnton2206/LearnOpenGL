#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "external/stb_image.h"
#include "shader.h"
#include "light.h"
#include "camera.h"

#include <random>
#include <iostream>
#include <string>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Shader& shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// Input
// Keys
bool isUpPressed = false;
bool isDownPressed = false;
bool isLeftPressed = false;
bool isRightPressed = false;
bool isWPressed = false;
bool isSPressed = false;
bool isAPressed = false;
bool isDPressed = false;

// Mouse
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Objects
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
DirectionLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f, 0.05f, 0.05f), 
						glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f));
SpotLight spotLights[1]; // NR_POINT_LIGHTS
PointLight pointLights[1]; // NR_SPOT_LIGHTS

Shader litShader;
Shader lightShader;

// Timings
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Cube Control
float mix = 0.2f;
float rotationSpeed = glm::radians(50.0f); // Cube rotation
float currentRotation = 0.0f;
float lastRotationTime = 0.0f;

// Light Control
glm::vec3 lightPos;
float lightOrbitRadius = 10.0f;

double random(float randomMax = 1.0f) {
	return randomMax * (double)rand() / (double)RAND_MAX;
}

int main() {
	glfwInit();
	// OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // -> borderless
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // resizing changes gl viewport
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ---------------------------------
	// light creation
	glm::vec3 ambient = glm::vec3(0.0f);

	glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 diffuseFlashlight = red * glm::vec3(2.0f);

	glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseLamp = white * glm::vec3(10.0f);

	spotLights[0] = SpotLight(0, glm::cos(glm::radians(5.5f)), glm::cos(glm::radians(8.5f)),
							  ambient, diffuseFlashlight, glm::vec3(1.0f, 1.0f, 1.0f),
							  camera.Position, camera.Front);

	pointLights[0] = PointLight(0, 1.0f, ambient, diffuseLamp, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f));

	// ---------------------------------
	// shaders file translation
	litShader = Shader("assets/shaders/lit/VertexShader.vert", "assets/shaders/lit/FragmentShader.frag");
	lightShader = Shader("assets/shaders/lighting/VertexShader.vert", "assets/shaders/lighting/FragmentShader.frag");

	// --------------------------------
	// texture
	unsigned int diffuseMap = loadTexture("assets/textures/container2.png");
	unsigned int specularMap = loadTexture("assets/textures/container2_specular.png");
	unsigned int emissionMap = loadTexture("assets/textures/container2_emission.jpg");

	litShader.use();

	// -----------------------------------
	// shaders settings
	litShader.setFloat("mix", mix);
	
	litShader.setInt("material.diffuse", 0);
	litShader.setInt("material.specular", 1);
	litShader.setInt("material.emission", 2);
	litShader.setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
	litShader.setFloat("material.shininess", 32.0f);

	// light
	dirLight.Setup(litShader);
	for (auto it = std::begin(spotLights); it != std::end(spotLights); ++it ) {
		it->SetupShader(litShader);
	}
	for (auto it = std::begin(pointLights); it != std::end(pointLights); ++it) {
		it->SetupShader(litShader);
	}

	//----------------------------------
	// transform
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(47.5f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.7, 0.2, 0.4));

	unsigned int transformLoc = glGetUniformLocation(litShader.ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// ---------------------------------
	// vertices

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// lit vertices
	unsigned int VBO, litVAO;
	glGenVertexArrays(1, &litVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(litVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// light vertices
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	
	glEnableVertexAttribArray(0);

	// ----------------------------
	// cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
	};

	glEnable(GL_DEPTH_TEST);

	// --------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window)) 
	{ 
		processInput(window, litShader);

		// settings
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// background color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// matrices
		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		// textures
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionMap);

		// light
		float lightX = sin(glfwGetTime()) * lightOrbitRadius;
		float lightZ = cos(glfwGetTime()) * lightOrbitRadius;
		lightPos = glm::vec3(lightX, 0.0f, lightZ);

		// light cube
		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		lightShader.setVec3("lightColor", white);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// lit shader
		litShader.use();
		litShader.setMat4("view", view);
		litShader.setMat4("projection", projection);
		litShader.setVec3("viewPos", camera.Position);

		spotLights[0].position = camera.Position;
		spotLights[0].direction = camera.Front;

		pointLights[0].position = lightPos;

		for (auto it = std::begin(spotLights); it != std::end(spotLights); ++it) {
			it->UpdateShader(litShader);
		}
		for (auto it = std::begin(pointLights); it != std::end(pointLights); ++it) {
			it->UpdateShader(litShader);
		}

		// mesh rendering
		glBindVertexArray(litVAO);
		for (unsigned int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			if ((i - 0) % 2 == 0) {
				model = glm::rotate(model, glm::radians(angle) + currentRotation + rotationSpeed * ((float)glfwGetTime() - lastRotationTime), glm::vec3(1.0f, 0.3f, 0.5f));
			}
			else {
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			}
			litShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window); 
		glfwPollEvents(); 
	}
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &litVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window, Shader& shader)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!isUpPressed) {
			isUpPressed = true;
			litShader.use();
			spotLights[0].diffuse.r += 1.0f;
			spotLights[0].SetupShader(litShader);
		}
	}
	else {
		isUpPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!isDownPressed) {
			isDownPressed = true;
			litShader.use();
			spotLights[0].diffuse.r -= 1.0f;
			spotLights[0].SetupShader(litShader);
		}
	}
	else {
		isDownPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (!isLeftPressed) {
			isLeftPressed = true;
			lightOrbitRadius -= 0.5f;
		}
	}
	else {
		isLeftPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (!isRightPressed) {
			isRightPressed = true;
			lightOrbitRadius += 0.5f;
		}
	}
	else {
		isRightPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		if (!isWPressed) {
			isWPressed = true;
		}
	}
	else {
		isWPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (!isSPressed) {
			isSPressed = true;
		}
	}
	else {
		isSPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		if (!isAPressed) {
			isAPressed = true;
		}
	}
	else {
		isAPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		if (!isDPressed) {
			isDPressed = true;
		}
	}
	else {
		isDPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	float xposF = static_cast<float>(xpos);
	float yposF = static_cast<float>(ypos);

	if (firstMouse)
	{
		lastX = xposF;
		lastY = yposF;
		firstMouse = false;
	}

	float xoffset = xposF - lastX;
	float yoffset = lastY - yposF;

	lastX = xposF;
	lastY = yposF;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{ 
	glViewport(0, 0, width, height); 
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}