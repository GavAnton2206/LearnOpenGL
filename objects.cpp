#include <glad/glad.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"

class Cube
{
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 rotation; // radians
    glm::vec3 scale;
    unsigned int texture1, texture2, texture3;
    unsigned int& VAO;

    Shader& shader;

    Cube(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_, 
        unsigned int& VAO_,
        Shader& shader_, 
        unsigned int texture1_ = 0, unsigned int texture2_ = 0, unsigned int texture3_ = 0) : shader(shader_), VAO(VAO_)
    {
        position = position_;
        rotation = rotation_;
        scale = scale_;
        texture1 = texture1_;
        texture2 = texture2_;
        texture3 = texture3_;
        velocity = glm::vec3(0.0);
    }

    glm::mat4 GetModelMatrix() {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        model = glm::scale(model, scale);
        
        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        return model;
    }

    void PhysicsProcess(glm::vec3 acceleration, float deltaTime) {
        velocity += acceleration * deltaTime;
        position += velocity;
    }

    void Draw() {
        shader.use();
        shader.setMat4("model", GetModelMatrix());
        
        if (texture1 != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
        }

        if (texture2 != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);
        }

        if (texture3 != 0) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture3);
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
private:
};

class Sphere
{
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 rotation; // radians
    glm::vec3 scale;
    unsigned int indexCount;
    unsigned int texture1, texture2, texture3;
    unsigned int& VAO;

    Shader& shader;

    Sphere(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
        unsigned int& VAO_,
        Shader& shader_,
        unsigned int indexCount_,
        unsigned int texture1_ = 0, unsigned int texture2_ = 0, unsigned int texture3_ = 0) : shader(shader_), VAO(VAO_)
    {
        position = position_;
        rotation = rotation_;
        scale = scale_;
        indexCount = indexCount_;
        texture1 = texture1_;
        texture2 = texture2_;
        texture3 = texture3_;
        velocity = glm::vec3(0.0);
    }

    glm::mat4 GetModelMatrix() {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);

        model = glm::scale(model, scale);

        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        return model;
    }

    void PhysicsProcess(glm::vec3 acceleration, float deltaTime) {
        velocity += acceleration * deltaTime;
        position += velocity;
    }

    void Draw() {
        shader.use();
        shader.setMat4("model", GetModelMatrix());

        if (texture1 != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
        }

        if (texture2 != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);
        }

        if (texture3 != 0) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture3);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
private:
};