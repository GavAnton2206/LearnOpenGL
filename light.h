#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "shader.h"

class DirectionLight
{
public:
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    bool shown;

    DirectionLight(glm::vec3 dir_ = glm::vec3(0.0f), glm::vec3 ambient_ = glm::vec3(0.0f), 
                   glm::vec3 diffuse_ = glm::vec3(0.0f), glm::vec3 specular_ = glm::vec3(0.0f)) :
                   direction(dir_), ambient(ambient_), diffuse(diffuse_), specular(specular_), shown(true), isShowing(true) {}

    void Setup(Shader& shader, bool use = false) {
        if (use)
            shader.use();

        shader.setVec3("dirLight.direction", direction);
        shader.setVec3("dirLight.ambient", ambient);
        shader.setVec3("dirLight.diffuse", diffuse);
        shader.setVec3("dirLight.specular", specular);
    }

    void Update(Shader& shader, bool use = false) {
        if (use)
            shader.use();

        if (isShowing && !shown) {
            shader.setVec3("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
            isShowing = false;
        }
        else if (!isShowing && shown) {
            shader.setVec3("dirLight.ambient", ambient);
            shader.setVec3("dirLight.diffuse", diffuse);
            isShowing = true;
        }

        shader.setVec3("dirLight.direction", direction);
    }
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


    SpotLight(int id_ = 0, float cutOff_ = glm::cos(glm::radians(15.5f)), float outerCutOff_ = glm::cos(glm::radians(18.5f)), glm::vec3 ambient_ = glm::vec3(0.0f), glm::vec3 diffuse_ = glm::vec3(0.0f),
              glm::vec3 specular_ = glm::vec3(0.0f), glm::vec3 position_ = glm::vec3(0.0f), 
              glm::vec3 dir_ = glm::vec3(0.0f)) :
        id(id_), cutOff(cutOff_), outerCutOff(outerCutOff_), direction(dir_), position(position_), ambient(ambient_), diffuse(diffuse_), specular(specular_), shown(true), isShowing(true) {}

    void Setup(Shader& shader, bool use = false, int id_ = -1) {
        if (use)
            shader.use();

        std::string Id;
        if(id_ == -1)
            Id = "spotLights[" + std::to_string(id) + "].";
        else
            Id = "spotLights[" + std::to_string(id_) + "].";

        shader.setVec3(Id + "position", position);
        shader.setVec3(Id + "direction", direction);

        shader.setFloat(Id + "cutOff", cutOff);
        shader.setFloat(Id + "outerCutOff", outerCutOff);

        shader.setVec3(Id + "ambient", ambient);
        shader.setVec3(Id + "diffuse", diffuse);
        shader.setVec3(Id + "specular", specular);
    }

    void Update(Shader& shader, bool use = false, int id_ = -1) {
        if (use)
            shader.use();

        std::string Id;
        if (id_ == -1)
            Id = "spotLights[" + std::to_string(id) + "].";
        else
            Id = "spotLights[" + std::to_string(id_) + "].";

        if (isShowing && !shown) {
            shader.setVec3(Id + "ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setVec3(Id + "diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
            isShowing = false;
        }
        else if (!isShowing && shown) {
            shader.setVec3(Id + "ambient", ambient);
            shader.setVec3(Id + "diffuse", diffuse);
            isShowing = true;
        }

        shader.setVec3(Id + "position", position);
        shader.setVec3(Id + "direction", direction);
    }
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

    PointLight(int id_ = 0, float radius = -1.0f, glm::vec3 ambient_ = glm::vec3(0.0f), glm::vec3 diffuse_ = glm::vec3(0.0f),
        glm::vec3 specular_ = glm::vec3(0.0f), glm::vec3 position_ = glm::vec3(0.0f), float constant_ = 1.0, float linear_ = 0.0, float quadratic_ = 0.0) :
        id(id_), constant(constant_), linear(linear_), quadratic(quadratic_), position(position_), ambient(ambient_), diffuse(diffuse_), specular(specular_), shown(true), isShowing(true)
    {
        if (radius != -1.0f && constant_ == 1.0f && linear_ == 0.0f && quadratic_ == 0.0f) {
            UpdateRadius(radius);
        }
    }

    void UpdateRadius(float radius) {
        float radiusLight = 0.01f;

        constant = 1.0f;
        linear = 0.22f;
        quadratic = 0.2f;
        //linear = (1 - radiusLight) / radiusLight * (2 / radius);
        //quadratic = (1 - radiusLight) / radiusLight * (1 / pow(radius, 2));
    }

    void Setup(Shader& shader, bool use = false, int id_ = -1) {
        if (use)
            shader.use();

        std::string Id;
        if (id_ == -1)
            Id = "pointLights[" + std::to_string(id) + "].";
        else
            Id = "pointLights[" + std::to_string(id_) + "].";

        shader.setVec3(Id + "position", position);

        shader.setFloat(Id + "constant", constant);
        shader.setFloat(Id + "linear", linear);
        shader.setFloat(Id + "quadratic", quadratic);

        shader.setVec3(Id + "ambient", ambient);
        shader.setVec3(Id + "diffuse", diffuse);
        shader.setVec3(Id + "specular", specular);
    }

    void Update(Shader& shader, bool use = false, int id_ = -1) {
        if (use)
            shader.use();

        std::string Id;
        if (id_ == -1)
            Id = "pointLights[" + std::to_string(id) + "].";
        else
            Id = "pointLights[" + std::to_string(id_) + "].";

        if (isShowing && !shown) {
            shader.setVec3(Id + "ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setVec3(Id + "diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
            isShowing = false;
        }
        else if (!isShowing && shown) {
            shader.setVec3(Id + "ambient", ambient);
            shader.setVec3(Id + "diffuse", diffuse);
            isShowing = true;
        }

        shader.setVec3(Id + "position", position);
    }
private:
    bool isShowing;
};
#endif