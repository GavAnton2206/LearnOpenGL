#pragma once

#include "objects.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);

void cubeMeshSetup(unsigned int& VBO, unsigned int& VAO);
void cubeMeshSetupX(unsigned int& VBO, unsigned int& VAO, glm::vec2 UV);
unsigned int sphereMeshSetup(unsigned int& sphereVBO, unsigned int& sphereVAO, unsigned int& sphereEBO, int stacks = 20, int sectors = 20);

CollisionInfo checkCollision(const SphereShape& s1, const SphereShape& s2);
CollisionInfo checkCollision(const SphereShape& s1, const AABBShape& s2);
CollisionInfo checkCollision(const AABBShape& s1, const SphereShape& s2);
CollisionInfo checkCollision(const AABBShape& s1, const AABBShape& s2);
CollisionInfo checkCollisions(const Rigidbody& obj1, const Rigidbody& obj2);

void DrawWithOutline(Object3D obj, Shader& shader_, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));

void resolveCollision(Rigidbody& A, Rigidbody& B, const CollisionInfo& info);
void resolveSpecialCollision(Rigidbody& A, Rigidbody& B, const CollisionInfo& info);

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
	GLsizei length, const char* message, const void* userParam);