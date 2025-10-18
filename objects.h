#pragma once

#include <glad/glad.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <variant>

#include "shader.h"

enum class ObjectType {
    DYNAMIC,
    KINEMATIC,
    STATIC
};

struct CollisionInfo {
    bool collided;
    glm::vec3 normal;
    float penetration;

    CollisionInfo(bool collided_ = false, glm::vec3 normal_ = glm::vec3(0.0f, 0.0f, 0.0f), float penetration_ = 0.0f);
};

struct SphereShape {
    glm::vec3 position;
    float radius;

    SphereShape();
    SphereShape(const glm::vec3& position_, const float& radius_);
};

struct AABBShape {
    glm::vec3 position;
    glm::vec3 halfSize;

    AABBShape();
    AABBShape(const glm::vec3& position_, const glm::vec3& size);

    glm::vec3 min() const;
    glm::vec3 max() const;
};

class Object3D
{
public:
    unsigned int indexCount;
    unsigned int texture1, texture2, texture3;
    glm::vec3 color;
    glm::vec2 UVScale;
    unsigned int& VAO;
    bool drawn;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Shader& shader;
    bool drawElements;

    Object3D(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
        unsigned int& VAO_,
        Shader& shader_,
        unsigned int indexCount_,
        bool drawElements_,
        unsigned int texture1_ = 0, unsigned int texture2_ = 0, unsigned int texture3_ = 0, 
        glm::vec2 UVScale_ = glm::vec2(1.0f), glm::vec3 color_ = glm::vec3(1.0f));

    Object3D& operator=(const Object3D& other);

    glm::mat4 GetModelMatrix();

    void Draw();

    void SetPosition(glm::vec3 position_);
    void SetPosition(float x, float y, float z);
    glm::vec3 GetPosition();

    void SetRotation(glm::vec3 rotation_);
    void SetRotation(float x, float y, float z);
    glm::vec3 GetRotation();

    void SetScale(glm::vec3 scale_);
    void SetScale(float x, float y, float z);
    glm::vec3 GetScale();
};

class Rigidbody : public Object3D {
public:
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float mass;
    std::variant<SphereShape, AABBShape> shape;
    ObjectType behavior;
    bool canCollide;

    Rigidbody(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
        unsigned int& VAO_,
        Shader& shader_,
        unsigned int indexCount_,
        bool drawElements_,
        float mass_,
        ObjectType type_ = ObjectType::DYNAMIC,
        //ShapeType shape_ = SphereShape(glm::vec3(0.0f), -1.0f),
        unsigned int texture1_ = 0, unsigned int texture2_ = 0, unsigned int texture3_ = 0,
        glm::vec2 UVScale = glm::vec2(1.0f), glm::vec3 color = glm::vec3(0.0f));

    Rigidbody& operator=(const Rigidbody& other);

    void ApplyForce(const glm::vec3& force);
    void PhysicsProcess(float deltaTime);

    void SetPosition(glm::vec3 position_);
    void SetPosition(float x, float y, float z);
    glm::vec3 GetPosition();

    void SetScale(glm::vec3 scale_);
    void SetScale(float x, float y, float z);
    glm::vec3 GetScale();
protected:
    friend bool operator==(const Rigidbody&, const Rigidbody&);
};

bool operator==(const Rigidbody& A, const Rigidbody& B);

