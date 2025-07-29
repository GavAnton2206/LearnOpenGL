#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "external/stb_image.h"
#include "shader.h"
#include "camera.h"

#include <random>
#include <iostream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Shader& shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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

// Settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Timings
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Cube Control
float mix = 0.2f;
float rotationSpeed = glm::radians(50.0f); // Cube rotation
float currentRotation = 0.0f;
float lastRotationTime = 0.0f;

// Light Control
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
	// shaders file translation
	Shader litShader("assets/shaders/lit/VertexShader.vert", "assets/shaders/lit/FragmentShader.frag");
	Shader lightShader("assets/shaders/lighting/VertexShader.vert", "assets/shaders/lighting/FragmentShader.frag");

	// --------------------------------
	// texture
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("assets/textures/container.jpg", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	data = stbi_load("assets/textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	litShader.use();

	// -----------------------------------
	// textures
	litShader.setInt("texture1", 0);
	litShader.setInt("texture2", 1);
	litShader.setFloat("mix", mix);

	litShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	litShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

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
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};

	// lit vertices
	unsigned int VBO, litVAO;
	glGenVertexArrays(1, &litVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(litVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// light vertices
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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

		// texture 1 and texture 2
		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		*/

		// light
		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// usage
		litShader.use();
		litShader.setMat4("view", view);
		litShader.setMat4("projection", projection);
		litShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		litShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		glBindVertexArray(litVAO);
		for (unsigned int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			if ((i - 1) % 2 == 0) {
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
			mix += 0.1f;
			if (mix > 1)
				mix = 1;
			shader.setFloat("mix", mix);
			shader.use();
		}
	}
	else {
		isUpPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (!isDownPressed) {
			isDownPressed = true;
			mix -= 0.1f;
			if (mix < 0)
				mix = 0;
			shader.setFloat("mix", mix);
			shader.use();
		}
	}
	else {
		isDownPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (!isLeftPressed) {
			isDownPressed = true;
			if (rotationSpeed < 0.0f) {
				currentRotation += rotationSpeed * ((float)glfwGetTime() - lastRotationTime);
				lastRotationTime = (float)glfwGetTime();
				rotationSpeed *= -1;
			}
		}
	}
	else {
		isLeftPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (!isRightPressed) {
			isRightPressed = true;
			if (rotationSpeed > 0.0f) {
				currentRotation += rotationSpeed * ((float)glfwGetTime() - lastRotationTime);
				lastRotationTime = (float)glfwGetTime();
				rotationSpeed *= -1;
			}
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

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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