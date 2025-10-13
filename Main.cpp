#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "external/stb_image.h"
#include "shader.h"
#include "camera.h"
#include "objects.h"
#include "light.h"

#include <random>
#include <iostream>
#include <string>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
void cubeMeshSetup(unsigned int& VBO, unsigned int& VAO);
void cubeMeshSetupX(unsigned int& VBO, unsigned int& VAO, glm::vec2 UV);
unsigned int sphereMeshSetup(unsigned int& sphereVBO, unsigned int& sphereVAO, unsigned int& sphereEBO, int stacks = 20, int sectors = 20);


CollisionInfo checkCollision(const SphereShape& s1, const SphereShape& s2);
CollisionInfo checkCollision(const SphereShape& s1, const AABBShape& s2);
CollisionInfo checkCollision(const AABBShape& s1, const SphereShape& s2);
CollisionInfo checkCollision(const AABBShape& s1, const AABBShape& s2);
CollisionInfo checkCollisions(const Rigidbody& obj1, const Rigidbody& obj2);

void resolveCollision(Rigidbody& A, Rigidbody& B, const CollisionInfo& info);
void resolveSpecialCollision(Rigidbody& A, Rigidbody& B, const CollisionInfo& info);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float PI = 3.1415926525897932384626433832;
const float rotationPerSecond = 0.25f;

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
bool isSpacePressed = false;
bool pause = false;
float cubeSpeed = 15.0f;

// Mouse
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Objects
std::vector<Rigidbody*> physicsObjects;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
DirectionLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.3f),
	glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f));
SpotLight spotLights[1]; // NR_POINT_LIGHTS
PointLight pointLights[1]; // NR_SPOT_LIGHTS

Shader litShader;
Shader litTexShader;
Shader lightShader;

std::vector<Rigidbody> bouncingObjects;
Rigidbody* ridingCube = nullptr;

glm::vec3 cubeControlPos;

// Timings
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Cube Control
float rotationSpeed = glm::radians(50.0f);
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

	glm::vec3 red = glm::vec3(1.0f, 0.0f, 1.0f);
	glm::vec3 diffuseFlashlight = red * glm::vec3(2.0f);

	glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuseLamp = white * glm::vec3(2.0f);

	dirLight = DirectionLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.1f),
		glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f));

	spotLights[0] = SpotLight(0, glm::cos(glm::radians(5.5f)), glm::cos(glm::radians(8.5f)),
		ambient, diffuseFlashlight, glm::vec3(1.0f, 1.0f, 1.0f),
		camera.Position, camera.Front);

	pointLights[0] = PointLight(0, 1.0f, ambient, diffuseLamp, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f));

	// ---------------------------------
	// shaders file translation
	litShader = Shader("assets/shaders/lit/VertexShader.vert", "assets/shaders/lit/FragmentShader.frag");
	litTexShader = Shader("assets/shaders/lit/VertexShaderTex.vert", "assets/shaders/lit/FragmentShaderTex.frag");
	lightShader = Shader("assets/shaders/lighting/VertexShader.vert", "assets/shaders/lighting/FragmentShader.frag");

	// --------------------------------
	// texture
	unsigned int boxDiffuseMap = loadTexture("assets/textures/container2.png");
	unsigned int boxSpecularMap = loadTexture("assets/textures/container2_specular.png");
	unsigned int boxEmissionMap = loadTexture("assets/textures/container2_emission.jpg");

	unsigned int boardsDiffuseMap = loadTexture("assets/textures/boards.png");
	unsigned int boardsSpecularMap = loadTexture("assets/textures/boards_specular.png");
	unsigned int boardsEmissionMap = loadTexture("assets/textures/boards_emission.jpg");

	// shader settings
	litTexShader.use();

	//litTexShader.setVec2("scaleUV", glm::vec2(1.0f));

	litTexShader.setInt("material.diffuse", 0);
	litTexShader.setInt("material.specular", 1);
	litTexShader.setInt("material.emission", 2);
	litTexShader.setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
	litTexShader.setFloat("material.shininess", 32.0f);

	// lit shader
	litShader.use();

	litShader.setVec3("material.diffuse", glm::vec3(0.5f));
	litShader.setVec3("material.specular", glm::vec3(0.0f));
	litShader.setVec3("material.emission", glm::vec3(0.0f));
	litShader.setFloat("material.shininess", 32.0f);

	// ---------------------------------
	// light setup
	dirLight.Setup(litShader, true);
	dirLight.Setup(litTexShader, true);
	dirLight.Setup(lightShader, true);
	for (auto it = std::begin(spotLights); it != std::end(spotLights); ++it) {
		it->Setup(litShader, true);
		it->Setup(litTexShader, true);
		it->Setup(lightShader, true);
	}
	for (auto it = std::begin(pointLights); it != std::end(pointLights); ++it) {
		it->Setup(litShader, true);
		it->Setup(litTexShader, true);
		it->Setup(lightShader, true);
	}

	// ---------------------------------
	// mesh setups
	unsigned int cubeVBO, cubeVAO;
	cubeMeshSetupX(cubeVBO, cubeVAO, glm::vec2(1.0f));

	unsigned int sphereVBO, sphereVAO, sphereEBO;
	unsigned int sphereVerticesNum = sphereMeshSetup(sphereVBO, sphereVAO, sphereEBO, 50, 50);

	// ----------------------------
	// cubes
	Object3D lightCube = Object3D(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(0.05f),
		cubeVAO,
		lightShader,
		36,
		false);

	Object3D floor = Object3D(glm::vec3(0.0f, -1.55f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(1000.0f, 0.01f, 1000.00f),
		cubeVAO,
		litTexShader,
		36,
		false,
		boardsDiffuseMap,
		boardsSpecularMap,
		boardsEmissionMap,
		glm::vec2(1000.0f));

	float density = 1.0f;
	glm::vec3 sphereColor = glm::vec3(1.0f, 1.0f, 0.0f);

	bouncingObjects.push_back(Rigidbody(glm::vec3(-6.0f, 3.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(0.545f),
		sphereVAO,
		litShader,
		sphereVerticesNum,
		true,
		4.0 / 3.0 * PI * pow(0.545f, 2) * density,
		ObjectType::DYNAMIC, 0, 0, 0, glm::vec2(0.0f), sphereColor));

	bouncingObjects.push_back(Rigidbody(glm::vec3(-6.0f, 7.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(0.545f),
		sphereVAO,
		litShader,
		sphereVerticesNum,
		true,
		4.0 / 3.0 * PI * pow(0.545f, 2) * density,
		ObjectType::DYNAMIC, 0, 0, 0, glm::vec2(0.0f), sphereColor));

	bouncingObjects.push_back(Rigidbody(glm::vec3(-6.0f, 10.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(0.545f),
		sphereVAO,
		litShader,
		sphereVerticesNum,
		true,
		4.0 / 3.0 * PI * pow(0.545f, 2) * density,
		ObjectType::DYNAMIC, 0, 0, 0, glm::vec2(0.0f), sphereColor));

	bouncingObjects.push_back(Rigidbody(glm::vec3(-6.0f, 1.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(0.545f),
		sphereVAO,
		litShader,
		sphereVerticesNum,
		true,
		4.0 / 3.0 * PI * pow(0.545f, 2) * density,
		ObjectType::DYNAMIC, 0, 0, 0, glm::vec2(0.0f), sphereColor));

	bouncingObjects.push_back(Rigidbody(glm::vec3(-10.0f, 4.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(0.545f),
		sphereVAO,
		litShader,
		sphereVerticesNum,
		true,
		4.0 / 3.0 * PI * pow(0.545f, 2) * density,
		ObjectType::DYNAMIC, 0, 0, 0, glm::vec2(0.0f), sphereColor));

	bouncingObjects.push_back(Rigidbody(glm::vec3(6.0f, 4.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(1.0f, 1.0f, 1.0f),
		cubeVAO,
		litTexShader,
		36,
		false,
		1.0f * density,
		ObjectType::DYNAMIC,
		boxDiffuseMap,
		boxSpecularMap,
		boxEmissionMap));

	bouncingObjects.push_back(Rigidbody(glm::vec3(10.0f, 6.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(1.0f, 1.0f, 1.0f),
		cubeVAO,
		litTexShader,
		36,
		false,
		1.0f * density,
		ObjectType::DYNAMIC,
		boxDiffuseMap,
		boxSpecularMap,
		boxEmissionMap));

	Rigidbody fallingCube = Rigidbody(glm::vec3(2.0f, 2.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(1.0f, 1.0f, 1.0f),
		cubeVAO,
		litTexShader,
		36,
		false,
		1.0f * density,
		ObjectType::DYNAMIC,
		boxDiffuseMap,
		boxSpecularMap,
		boxEmissionMap);

	ridingCube = new Rigidbody(glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(1.0f, 1.0f, 1.0f),
		cubeVAO,
		litTexShader,
		36,
		false,
		1.0f * density,
		ObjectType::KINEMATIC,
		boxDiffuseMap,
		boxSpecularMap,
		boxEmissionMap);

	Rigidbody fallingSphere = Rigidbody(glm::vec3(-2.0f, 2.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(0.545f),
		sphereVAO,
		litShader,
		sphereVerticesNum,
		true,
		4.0 / 3.0 * PI * pow(0.545f, 2) * density,
		ObjectType::DYNAMIC, 0, 0, 0, glm::vec2(0.0f), sphereColor);

	// Physics Objects
	for (unsigned int i = 0; i < bouncingObjects.size(); i++)
	{
		physicsObjects.push_back(&bouncingObjects[i]);
	}

	physicsObjects.push_back(&fallingCube);
	physicsObjects.push_back(&fallingSphere);
	physicsObjects.push_back(ridingCube);

	glEnable(GL_DEPTH_TEST);

	// --------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

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

		// light and shaders
		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		lightShader.setVec3("lightColor", white);

		litTexShader.use();
		litTexShader.setMat4("view", view);
		litTexShader.setMat4("projection", projection);
		litTexShader.setVec3("viewPos", camera.Position);

		litShader.use();
		litShader.setMat4("view", view);
		litShader.setMat4("projection", projection);
		litShader.setVec3("viewPos", camera.Position);

		spotLights[0].position = camera.Position;
		spotLights[0].direction = camera.Front;

		pointLights[0].position = lightPos;

		int j = 0;
		for (auto it = std::begin(spotLights); it != std::end(spotLights); ++it, j++) {
			it->Update(litShader, true, j);
			it->Update(litTexShader, true, j);
			it->Update(lightShader, true, j);
		}
		j = 0;
		for (auto it = std::begin(pointLights); it != std::end(pointLights); ++it, j++) {
			it->Update(litShader, true, j);
			it->Update(litTexShader, true, j);
			it->Update(lightShader, true, j);
		}


		// movement
		// light cube
		float lightX = sin(glfwGetTime()) * lightOrbitRadius;
		float lightZ = cos(glfwGetTime()) * lightOrbitRadius;
		lightPos = glm::vec3(lightX, 0.0f, lightZ);

		lightCube.SetPosition(lightPos);

		lightCube.Draw();

		// Bouncing Objects
	
		for (unsigned int i = 0; i < bouncingObjects.size(); i++) {
			if (bouncingObjects[i].GetPosition().y <= -1.0f) {
				if (bouncingObjects[i].GetPosition().y < 0)
					bouncingObjects[i].velocity.y *= -0.9f;

				if (bouncingObjects[i].velocity.y < 0.001f) {
					bouncingObjects[i].velocity.y = 0.0f;
					bouncingObjects[i].SetRotation(bouncingObjects[i].GetRotation().x, bouncingObjects[i].GetRotation().y, 0.0f);
				}
			}
		}

		// Resetting Cube and Sphere
		if (fallingCube.GetPosition().y <= -3.0f) {
			fallingCube.SetPosition(fallingCube.GetPosition().x, 10.0f + random(5.0f), fallingCube.GetPosition().z);
			fallingCube.velocity = glm::vec3(0.0f);
		}

		if (fallingSphere.GetPosition().y <= -3.0f) {
			fallingSphere.SetPosition(fallingSphere.GetPosition().x, 10.0f + random(5.0f), fallingSphere.GetPosition().z);
			fallingSphere.velocity = glm::vec3(0.0f);
		}

		// General Physics

		for (unsigned int i = 0; i < physicsObjects.size(); i++)
		{
			if (!pause) {
				if (physicsObjects[i]->behavior == ObjectType::DYNAMIC)
					physicsObjects[i]->ApplyForce(glm::vec3(0.0f, -0.0098f, 0.0f));
				
				for (unsigned int j = i + 1; j < physicsObjects.size(); j++)
				{
					CollisionInfo info = checkCollisions(*physicsObjects[i], *physicsObjects[j]);

					if (info.collided) {
						resolveCollision(*physicsObjects[i], *physicsObjects[j], info);
					}
				}

				physicsObjects[i]->PhysicsProcess(deltaTime);
			}
			physicsObjects[i]->Draw();
		}

		floor.Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &sphereVAO);
	glDeleteBuffers(1, &sphereVBO);
	glDeleteBuffers(1, &sphereEBO);
	glDeleteTextures(1, &boxDiffuseMap);
	glDeleteTextures(1, &boxSpecularMap);
	glDeleteTextures(1, &boxEmissionMap);
	glDeleteTextures(1, &boardsDiffuseMap);
	glDeleteTextures(1, &boardsSpecularMap);
	glDeleteTextures(1, &boardsEmissionMap);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (!isUpPressed) {
			isUpPressed = true;
			spotLights[0].diffuse.r += 1.0f;
			spotLights[0].Setup(litTexShader, true);
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
			spotLights[0].Setup(litTexShader, true);
		}
	}
	else {
		isDownPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (!isLeftPressed) {
			isLeftPressed = true;
		}
		ridingCube->SetPosition(ridingCube->GetPosition().x - cubeSpeed * deltaTime, ridingCube->GetPosition().y, ridingCube->GetPosition().z);
	}
	else {
		isLeftPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (!isRightPressed) {
			isRightPressed = true;
		}
		ridingCube->SetPosition(ridingCube->GetPosition().x + cubeSpeed * deltaTime, ridingCube->GetPosition().y, ridingCube->GetPosition().z);
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

void cubeMeshSetupX(unsigned int& cubeVBO, unsigned int& cubeVAO, glm::vec2 UV) {
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  UV.x, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  UV.x, UV.y,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  UV.x, UV.y,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, UV.y,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   UV.x, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   UV.x, UV.y,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   UV.x, UV.y,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, UV.y,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  UV.x, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  UV.x, UV.y,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, UV.y,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, UV.y,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  UV.x, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  UV.x, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  UV.x, UV.y,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, UV.y,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, UV.y,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  UV.x, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, UV.y,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  UV.x, UV.y,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  UV.x, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  UV.x, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, UV.y,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, UV.y,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  UV.x, UV.y,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  UV.x, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  UV.x, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, UV.y
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void cubeMeshSetup(unsigned int& cubeVBO, unsigned int& cubeVAO) {
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

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

unsigned int sphereMeshSetup(unsigned int& sphereVBO, unsigned int& sphereVAO, unsigned int& sphereEBO, int stacks, int sectors) {
	std::vector<float> vertices;

	for (int i = 0; i <= stacks; ++i) {
		float stackAngle = PI / 2.0f - i * (PI / stacks); // from pi/2 to -pi/2
		float xy = cosf(stackAngle);                     // r * cos(u)
		float z = sinf(stackAngle);                      // r * sin(u)

		for (int j = 0; j <= sectors; ++j) {
			float sectorAngle = j * (2 * PI / sectors); // from 0 to 2pi

			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);

			// normalized position = normal for a sphere
			float nx = x;
			float ny = y;
			float nz = z;

			float u = (float)j / sectors;
			float v = (float)i / stacks;

			// position + normal + texcoord (8 floats)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			vertices.push_back(nx);
			vertices.push_back(ny);
			vertices.push_back(nz);

			vertices.push_back(u);
			vertices.push_back(v);
		}
	}

	std::vector<unsigned int> indices;
	for (int i = 0; i < stacks; ++i) {
		int k1 = i * (sectors + 1);     // beginning of current stack
		int k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
			if (i != 0) {
				// upper triangle
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != (stacks - 1)) {
				// lower triangle
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &sphereEBO);

	glBindVertexArray(sphereVAO);

	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Vertex attributes: position (0), normal (1), texCoord (2)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	return static_cast<unsigned int>(indices.size());
}

CollisionInfo checkCollision(const SphereShape& obj1, const SphereShape& obj2) {
	CollisionInfo info;

	glm::vec3 posDiff = obj1.position - obj2.position;
	float dist = glm::length(obj1.position - obj2.position);
	float r = obj1.radius + obj2.radius;
	if (dist < r) {
		info.collided = true;
		info.penetration = r - dist;
		info.normal = (dist > 0.0f) ? posDiff / dist : glm::vec3(1, 0, 0);
	}

	return info;
}

CollisionInfo checkCollision(const SphereShape& obj1, const AABBShape& obj2) {
	CollisionInfo info;

	glm::vec3 closestPoint = glm::clamp(obj1.position, obj2.min(), obj2.max());
	glm::vec3 posDiff = obj1.position - closestPoint;
	float dist = glm::length(posDiff);

	if (dist < obj1.radius) {
		info.collided = true;
		info.penetration = obj1.radius - dist;
		info.normal = (dist > 0.0f) ? posDiff / dist : glm::vec3(1, 0, 0);
	}

	return info;
}

CollisionInfo checkCollision(const AABBShape& obj1, const SphereShape& obj2) {
	return checkCollision(obj2, obj1);
}

CollisionInfo checkCollision(const AABBShape& obj1, const AABBShape& obj2) {
	CollisionInfo info;

	glm::vec3 minA = obj1.min();
	glm::vec3 minB = obj2.min();
	glm::vec3 maxA = obj1.max();
	glm::vec3 maxB = obj2.max();

	if ((minA.x <= maxB.x && maxA.x >= minB.x) &&
		(minA.y <= maxB.y && maxA.y >= minB.y) &&
		(minA.z <= maxB.z && maxA.z >= minB.z))
	{
		info.collided = true;


		float overlapX = std::min(maxA.x, maxB.x) - std::max(minA.x, minB.x);
		float overlapY = std::min(maxA.y, maxB.y) - std::max(minA.y, minB.y);
		float overlapZ = std::min(maxA.z, maxB.z) - std::max(minA.z, minB.z);

		if (overlapX < overlapY && overlapX < overlapZ) {
			info.penetration = overlapX;
			info.normal = (obj1.position.x < obj2.position.x) ? glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0);
		}
		else if (overlapY < overlapZ) {
			info.penetration = overlapY;
			info.normal = (obj1.position.y < obj2.position.y) ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0);
		}
		else {
			info.penetration = overlapZ;
			info.normal = (obj1.position.z < obj2.position.z) ? glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1);
		}
	}

	return info;
}

CollisionInfo checkCollisions(const Rigidbody& obj1, const Rigidbody& obj2) {
	CollisionInfo info;

	if (!obj1.canCollide || !obj2.canCollide)
		return info;

	return std::visit([](auto&& s1, auto&& s2) { return checkCollision(s1, s2); }, obj1.shape, obj2.shape);
}

void resolveCollision(Rigidbody& A, Rigidbody& B, const CollisionInfo& info) {
	if (!info.collided) return;

	resolveSpecialCollision(A, B, info);

	if (A.behavior != ObjectType::DYNAMIC && B.behavior != ObjectType::DYNAMIC)
		return;

	glm::vec3 normal = glm::normalize(info.normal);

	glm::vec3 relativeVelocity = A.velocity - B.velocity;

	if (A.behavior != ObjectType::DYNAMIC)
		glm::vec3 relativeVelocity = -B.velocity;
	else if (B.behavior != ObjectType::DYNAMIC)
		glm::vec3 relativeVelocity = A.velocity;

	float velAlongNormal = glm::dot(relativeVelocity, normal);

	if (velAlongNormal > 0.0f) return;

	float restitution = 1.0f;

	float invMassA = (A.mass > 0.0f) ? (1.0f / A.mass) : 0.0f;
	float invMassB = (B.mass > 0.0f) ? (1.0f / B.mass) : 0.0f;

	float j = -(1.0f + restitution) * velAlongNormal;
	j /= (invMassA + invMassB);

	glm::vec3 impulse = j * normal;

	if (A.behavior == ObjectType::DYNAMIC)
		A.velocity += (impulse * invMassA);

	if (B.behavior == ObjectType::DYNAMIC)
		B.velocity -= (impulse * invMassB);

	const float percent = 0.2f;
	const float slop = 0.01f;

	glm::vec3 correction = (std::max(info.penetration - slop, 0.0f) / (invMassA + invMassB)) * percent * normal;

	if (A.behavior == ObjectType::DYNAMIC)
		A.SetPosition(A.GetPosition() - invMassA * correction);
	if (B.behavior == ObjectType::DYNAMIC)
		B.SetPosition(B.GetPosition() + invMassB * correction);
}

void resolveSpecialCollision(Rigidbody& A, Rigidbody& B, const CollisionInfo& info) {
	if (A == *ridingCube) {
		//B.drawn = false;
		//B.canCollide = false;
	}

	if (B == *ridingCube) {
		//A.drawn = false;
		//A.canCollide = false;
	}
}