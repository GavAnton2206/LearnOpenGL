#include <glad/glad.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <variant>

#include "shader.h"

struct CollisionInfo {
    bool collided;
    glm::vec3 normal;
    float penetration;

    CollisionInfo(bool collided_ = false, glm::vec3 normal_ = glm::vec3(0.0f, 0.0f, 0.0f), float penetration_ = 0.0f):
        collided(collided_), normal(normal_), penetration(penetration_)
    {

    }
};

struct SphereShape {
    glm::vec3 position;
    float radius;

    SphereShape() = default;
    SphereShape(const glm::vec3& position_, const float& radius_) : position(position_), radius(radius_) {}
};

struct AABBShape {
    glm::vec3 position;
    glm::vec3 halfSize;

    AABBShape() = default;
    AABBShape(const glm::vec3& position_, const glm::vec3& size) : position(position_), halfSize(size / 2.0f) {}

    glm::vec3 min() const {
        return position - halfSize;
    }

    glm::vec3 max() const {
        return position + halfSize;
    }
};

class Object3D
{
public:
    unsigned int indexCount;
    unsigned int texture1, texture2, texture3;
    unsigned int& VAO;

    Shader& shader;
    bool drawElements;
    bool drawn;

    Object3D(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
        unsigned int& VAO_,
        Shader& shader_,
        unsigned int indexCount_,
        bool drawElements_,
        unsigned int texture1_ = 0, unsigned int texture2_ = 0, unsigned int texture3_ = 0) : shader(shader_), VAO(VAO_)
    {
        position = position_;
        rotation = rotation_;
        scale = scale_;
        indexCount = indexCount_;
        texture1 = texture1_;
        texture2 = texture2_;
        texture3 = texture3_;
        drawElements = drawElements_;
        drawn = true;
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

    void Draw() {
        if (!drawn)
            return;

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

        if (!drawElements) {
            glDrawArrays(GL_TRIANGLES, 0, indexCount);
        }
        else {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }
    }

    void SetPosition(glm::vec3 position_) {
        position = position_;
    }
    void SetPosition(float x, float y, float z) {
        position = glm::vec3(x, y, z);
    }
    glm::vec3 GetPosition() {
        return position;
    }

    void SetRotation(glm::vec3 rotation_) {
        rotation = rotation_;
    }
    void SetRotation(float x, float y, float z) {
        rotation = glm::vec3(x, y, z);
    }
    glm::vec3 GetRotation() {
        return rotation;
    }
    
    void SetScale(glm::vec3 scale_) {
        scale = scale_;
    }
    void SetScale(float x, float y, float z) {
        scale = glm::vec3(x, y, z);
    }
    glm::vec3 GetScale() {
        return scale;
    }
protected:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class Rigidbody : public Object3D {
public:
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float mass;
    std::variant<SphereShape, AABBShape> shape;

    Rigidbody(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
        unsigned int& VAO_,
        Shader& shader_,
        unsigned int indexCount_,
        bool drawElements_,
        float mass_,
        //ShapeType shape_ = SphereShape(glm::vec3(0.0f), -1.0f),
        unsigned int texture1_ = 0, unsigned int texture2_ = 0, unsigned int texture3_ = 0) : Object3D(position_, rotation_, scale_, VAO_, shader_, indexCount_, drawElements_, texture1_, texture2_, texture3_)
    {
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        mass = mass_;

        if (drawElements_) { // sphere
            shape = SphereShape(position_, scale_.x);
        }
        else {
            shape = AABBShape(position_, scale_);
        }
    }

    void ApplyForce(const glm::vec3& force) {
        acceleration += force / mass;
    }

    void PhysicsProcess(float deltaTime) {
        velocity += acceleration * deltaTime;
        SetPosition(position + velocity);

        acceleration = glm::vec3(0.0f);
    }

    void SetPosition(glm::vec3 position_) {
        position = position_;
        std::visit([&position_](auto& s) {
            s.position = position_;
        }, shape);
    }
    void SetPosition(float x, float y, float z) {
        position = glm::vec3(x, y, z);
        std::visit([&x, &y, &z](auto& s) {
            s.position = glm::vec3(x, y, z);
        }, shape);
    }
    glm::vec3 GetPosition() {
        return position;
    }

    void SetScale(glm::vec3 scale_) {
        scale = scale_;
        std::visit([&scale_](auto& s) {
            if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, AABBShape>) {
                s.halfSize = scale_/2.0f;
            }
            if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, SphereShape>) {
                s.radius = scale_.x;
            }
        }, shape);
    }
    void SetScale(float x, float y, float z) {
        scale = glm::vec3(x, y, z);
        std::visit([&x, &y, &z](auto& s) {
            if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, AABBShape>) {
                s.halfSize = glm::vec3(x, y, z) / 2.0f;
            }
            if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, SphereShape>) {
                s.radius = x;
            }
        }, shape);
    }
    glm::vec3 GetScale() {
        return scale;
    }
};