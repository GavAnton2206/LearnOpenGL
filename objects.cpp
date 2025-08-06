#include <glad/glad.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"

class Cube
{
public:
    glm::vec3 position;
    glm::vec3 rotation; // radians
    glm::vec3 scale;

    Shader& shader;

    Cube(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_, Shader& shader_) : shader(shader_)
    {
        position = position_;
        rotation = rotation_;
        scale = scale_;
    }

    glm::mat4 GetModelMatrix() {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);
        model = glm::translate(model, position);

        return model;
    }

    void Draw() {
        shader.use();
        shader.setMat4("model", GetModelMatrix());

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
private:
};