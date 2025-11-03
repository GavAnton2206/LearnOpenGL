#pragma once

#include "shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class DirectionLight
{
public:
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    bool shown;

    DirectionLight(glm::vec3 dir_ = glm::vec3(0.0f),
                   glm::vec3 ambient_ = glm::vec3(0.0f),
                   glm::vec3 diffuse_ = glm::vec3(0.0f),
                   glm::vec3 specular_ = glm::vec3(0.0f));

    void Setup(Shader& shader, bool use = false);

    void Update(Shader& shader, bool use = false);
    void Update(std::vector<Shader>& shaders, bool use = false, int id_ = -1);

private:
    bool isShowing;
};


class SpotLight
{
public:
    int id;
    glm::vec3  position;
    glm::vec3  direction;
    float cutOff;
    float outerCutOff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    bool shown;


    SpotLight(int id_ = 0,
        float cutOff_ = glm::cos(glm::radians(15.5f)),
        float outerCutOff_ = glm::cos(glm::radians(18.5f)),
        glm::vec3 ambient_ = glm::vec3(0.0f),
        glm::vec3 diffuse_ = glm::vec3(0.0f),
        glm::vec3 specular_ = glm::vec3(0.0f),
        glm::vec3 position_ = glm::vec3(0.0f),
        glm::vec3 dir_ = glm::vec3(0.0f));

    void Setup(Shader& shader, bool use = false, int id_ = -1);

    void Update(Shader& shader, bool use = false, int id_ = -1);
    void Update(std::vector<Shader>& shaders, bool use = false, int id_ = -1);

private:
    bool isShowing;
};


class PointLight
{
public:
    int id;

    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    bool shown;

    PointLight(int id_ = 0,
        float radius = -1.0f,
        glm::vec3 ambient_ = glm::vec3(0.0f),
        glm::vec3 diffuse_ = glm::vec3(0.0f),
        glm::vec3 specular_ = glm::vec3(0.0f),
        glm::vec3 position_ = glm::vec3(0.0f),
        float constant_ = 1.0,
        float linear_ = 0.0,
        float quadratic_ = 0.0);

    void UpdateRadius(float radius);

    void Setup(Shader& shader, bool use = false, int id_ = -1);

    void Update(Shader& shader, bool use = false, int id_ = -1);
    void Update(std::vector<Shader>& shaders, bool use = false, int id_ = -1);

private:
    bool isShowing;
};