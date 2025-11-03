#include "light.h"

DirectionLight::DirectionLight(glm::vec3 dir_, glm::vec3 ambient_, glm::vec3 diffuse_, glm::vec3 specular_) :
        direction(dir_), 
        ambient(ambient_), 
        diffuse(diffuse_), 
        specular(specular_), 
        shown(true), 
        isShowing(true) {}

void DirectionLight::Setup(Shader& shader, bool use) {
    if (use)
        shader.use();

    shader.setVec3("dirLight.direction", direction);
    shader.setVec3("dirLight.ambient", ambient);
    shader.setVec3("dirLight.diffuse", diffuse);
    shader.setVec3("dirLight.specular", specular);
}

void DirectionLight::Update(Shader& shader, bool use) {
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

void DirectionLight::Update(std::vector<Shader>& shaders, bool use, int id_) {
    for (Shader shader : shaders)
    {
        if (use) {
            shader.use();
        }

        if (isShowing && !shown) {
            shader.setVec3("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else if (!isShowing && shown) {
            shader.setVec3("dirLight.ambient", ambient);
            shader.setVec3("dirLight.diffuse", diffuse);
        }
    }

    if (isShowing && !shown) {
        isShowing = false;
    }
    else if (!isShowing && shown) {
        isShowing = true;
    }
}


SpotLight::SpotLight(int id_, float cutOff_, float outerCutOff_, glm::vec3 ambient_, glm::vec3 diffuse_,
                     glm::vec3 specular_, glm::vec3 position_, glm::vec3 dir_) 
               : id(id_), 
                 cutOff(cutOff_), 
                 outerCutOff(outerCutOff_), 
                 direction(dir_), 
                 position(position_), 
                 ambient(ambient_), 
                 diffuse(diffuse_), 
                 specular(specular_), 
                 shown(true), 
                 isShowing(true) {}

void SpotLight::Setup(Shader& shader, bool use, int id_) {
    if (use)
        shader.use();

    std::string Id;
    if (id_ == -1)
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

void SpotLight::Update(Shader& shader, bool use, int id_) {
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

void SpotLight::Update(std::vector<Shader>& shaders, bool use, int id_) {
    for (Shader shader : shaders)
    {
        if (use) {
            shader.use();
        }

        std::string Id;
        if (id_ == -1)
            Id = "spotLights[" + std::to_string(id) + "].";
        else
            Id = "spotLights[" + std::to_string(id_) + "].";

        if (isShowing && !shown) {
            shader.setVec3(Id + "ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setVec3(Id + "diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else if (!isShowing && shown) {
            shader.setVec3(Id + "ambient", ambient);
            shader.setVec3(Id + "diffuse", diffuse);
        }

        shader.setVec3(Id + "position", position);
        shader.setVec3(Id + "direction", direction);
    }

    if (isShowing && !shown) {
        isShowing = false;
    }
    else if (!isShowing && shown) {
        isShowing = true;
    }
}


PointLight::PointLight(int id_, float radius, glm::vec3 ambient_, glm::vec3 diffuse_,
        glm::vec3 specular_, glm::vec3 position_, float constant_, float linear_, float quadratic_) 
    : id(id_), 
    constant(constant_), 
    linear(linear_), 
    quadratic(quadratic_), 
    position(position_), 
    ambient(ambient_), 
    diffuse(diffuse_), 
    specular(specular_), 
    shown(true), 
    isShowing(true)
    {
        if (radius != -1.0f && constant_ == 1.0f && linear_ == 0.0f && quadratic_ == 0.0f) {
            UpdateRadius(radius);
        }
    }

void PointLight::UpdateRadius(float radius) {
    float radiusLight = 0.01f;

    constant = 1.0f;
    linear = 0.22f;
    quadratic = 0.2f;
    //linear = (1 - radiusLight) / radiusLight * (2 / radius);
    //quadratic = (1 - radiusLight) / radiusLight * (1 / pow(radius, 2));
}

void PointLight::Setup(Shader& shader, bool use, int id_) {
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

void PointLight::Update(Shader& shader, bool use, int id_) {
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

void PointLight::Update(std::vector<Shader>& shaders, bool use, int id_) {
    for (Shader shader : shaders)
    {
        if (use) {
            shader.use();
        }

        std::string Id;
        if (id_ == -1)
            Id = "spotLights[" + std::to_string(id) + "].";
        else
            Id = "spotLights[" + std::to_string(id_) + "].";

        if (isShowing && !shown) {
            shader.setVec3(Id + "ambient", glm::vec3(0.0f, 0.0f, 0.0f));
            shader.setVec3(Id + "diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else if (!isShowing && shown) {
            shader.setVec3(Id + "ambient", ambient);
            shader.setVec3(Id + "diffuse", diffuse);
        }

        shader.setVec3(Id + "position", position);
    }

    if (isShowing && !shown) {
        isShowing = false;
    }
    else if (!isShowing && shown) {
        isShowing = true;
    }
}