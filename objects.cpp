#include <glad/glad.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"

enum class ShapeType {
    SPHERE,
    AABB
};

class Shape {
public:
    ShapeType type;
    glm::vec3 position;
    
    virtual ~Shape() = default;
};

class SphereShape : public Shape {
public:
    float radius;

    SphereShape(float radius_, const glm::vec3& position_) {
        radius = radius_;
        position = position_;
        type = ShapeType::SPHERE;
    }
};
class AABB : public Shape {
public:
    glm::vec3 halfSize;

    AABB(const glm::vec3& size, const glm::vec3& position_) {
        halfSize = size/2.0f;
        position = position_;
        type = ShapeType::AABB;
    }

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
    glm::vec3 position;
    glm::vec3 rotation; // radians
    glm::vec3 scale;
    unsigned int indexCount;
    unsigned int texture1, texture2, texture3;
    unsigned int& VAO;

    Shader& shader;
    bool drawElements;

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
private:
};

class Rigidbody : public Object3D {
public:
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float mass;
    const Shape* shape;

    Rigidbody(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
        unsigned int& VAO_,
        Shader& shader_,
        unsigned int indexCount_,
        bool drawElements_,
        float mass_,
        //const Shape* shape_,
        unsigned int texture1_ = 0, unsigned int texture2_ = 0, unsigned int texture3_ = 0) : Object3D(position_, rotation_, scale_, VAO_, shader_, indexCount_, drawElements_, texture1_, texture2_, texture3_)// , shape(shape_)
    {
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        mass = mass_;
    }

    void ApplyForce(const glm::vec3& force) {
        acceleration += force / mass;
    }

    void PhysicsProcess(float deltaTime) {
        velocity += acceleration * deltaTime;
        position += velocity;

        acceleration = glm::vec3(0.0f);
    }
};

/*
bool checkCollision(const Shape& obj1, const Shape& obj2) {
    if (obj1.type == ShapeType::SPHERE && obj2.type == ShapeType::SPHERE) {
        const SphereShape* sphere1 = dynamic_cast<const SphereShape*>(&obj1);
        const SphereShape* sphere2 = dynamic_cast<const SphereShape*>(&obj2);
        if (sphere1 && sphere2) {
            float dist = glm::length(sphere1->position - sphere2->position);
            float radius = sphere1->radius + sphere2->radius;
            return dist < radius;
        }
    }
    else if (obj1.type == ShapeType::SPHERE && obj2.type == ShapeType::AABB) {
        const SphereShape* sphere = dynamic_cast<const SphereShape*>(&obj1);
        const AABB* box = dynamic_cast<const AABB*>(&obj2);
        if (sphere && box) {
            return checkCollisionSphereAABB(sphere, box);
        }
    }
    else if (obj1.type == ShapeType::AABB && obj2.type == ShapeType::SPHERE) {
        const SphereShape* sphere = dynamic_cast<const SphereShape*>(&obj2);
        const AABB* box = dynamic_cast<const AABB*>(&obj1);
        if (sphere && box) {
            return checkCollisionSphereAABB(sphere, box);
        }
    }
    else if (obj1.type == ShapeType::AABB && obj2.type == ShapeType::AABB) {
        const AABB* a = dynamic_cast<const AABB*>(&obj1);
        const AABB* b = dynamic_cast<const AABB*>(&obj2);
        if (a && b) {
            return (a->min().x <= b->max().x && a->max().x >= b->min().x) &&
                (a->min().y <= b->max().y && a->max().y >= b->min().y) &&
                (a->min().z <= b->max().z && a->max().z >= b->min().z);
        }
    }
}

bool checkCollisionSphereAABB(const SphereShape* sphere, const AABB* aabb) {
    glm::vec3 clamped = glm::clamp(sphere->position, aabb->min(), aabb->max());
    float distSq = glm::length(clamped - sphere->position);
    return distSq < sphere->radius;
}
*/