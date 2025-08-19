#include "objects.h"

#pragma region CollisionInfo Methods
CollisionInfo::CollisionInfo(bool collided_, glm::vec3 normal_, float penetration_):
        collided(collided_), normal(normal_), penetration(penetration_) {}
#pragma endregion

#pragma region SphereShape Methods
SphereShape::SphereShape() = default;
SphereShape::SphereShape(const glm::vec3& position_, const float& radius_) : position(position_), radius(radius_) {}
#pragma endregion

#pragma region AABBShape Methods
AABBShape::AABBShape() = default;
AABBShape::AABBShape(const glm::vec3& position_, const glm::vec3& size) : position(position_), halfSize(size / 2.0f) {}
glm::vec3 AABBShape::min() const {
    return position - halfSize;
}

glm::vec3 AABBShape::max() const {
    return position + halfSize;
}
#pragma endregion

#pragma region Object3D Methods
Object3D::Object3D(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
    unsigned int& VAO_,
    Shader& shader_,
    unsigned int indexCount_,
    bool drawElements_,
    unsigned int texture1_, unsigned int texture2_, unsigned int texture3_) : shader(shader_), VAO(VAO_)
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
};

Object3D& Object3D::operator=(const Object3D& other) {
    if (this == &other) return *this; // self-assignment guard

    this->indexCount = other.indexCount;
    this->texture1 = other.texture1;
    this->texture2 = other.texture2;
    this->texture3 = other.texture3;
    this->VAO = other.VAO;
    this->shader = other.shader;
    this->drawElements = other.drawElements;
    this->drawn = other.drawn;

    return *this;
}

glm::mat4 Object3D::GetModelMatrix() {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);

    model = glm::scale(model, scale);

    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
}

void Object3D::Draw() {
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

void Object3D::SetPosition(glm::vec3 position_) {
    position = position_;
}
void Object3D::SetPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
}
glm::vec3 Object3D::GetPosition() {
    return position;
}

void Object3D::SetRotation(glm::vec3 rotation_) {
    rotation = rotation_;
}
void Object3D::SetRotation(float x, float y, float z) {
    rotation = glm::vec3(x, y, z);
}
glm::vec3 Object3D::GetRotation() {
    return rotation;
}
    
void Object3D::SetScale(glm::vec3 scale_) {
    scale = scale_;
}
void Object3D::SetScale(float x, float y, float z) {
    scale = glm::vec3(x, y, z);
}
glm::vec3 Object3D::GetScale() {
    return scale;
}
#pragma endregion

#pragma region Rigidbody Methods
Rigidbody::Rigidbody(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_,
        unsigned int& VAO_,
        Shader& shader_,
        unsigned int indexCount_,
        bool drawElements_,
        float mass_,
        ObjectType type_,
        //ShapeType shape_ = SphereShape(glm::vec3(0.0f), -1.0f),
        unsigned int texture1_, unsigned int texture2_, unsigned int texture3_) : Object3D(position_, rotation_, scale_, VAO_, shader_, indexCount_, drawElements_, texture1_, texture2_, texture3_)
    {
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        mass = mass_;
        behavior = type_;

        if (drawElements_) { // sphere
            shape = SphereShape(position_, scale_.x);
        }
        else {
            shape = AABBShape(position_, scale_);
        }
    }

Rigidbody& Rigidbody::operator=(const Rigidbody& other) {
        if (this == &other) return *this; // self-assignment guard

        this->acceleration = other.acceleration;
        this->velocity = other.velocity;
        this->mass = other.mass;
        this->shape = other.shape;
        this->behavior = other.behavior;

        this->indexCount = other.indexCount;
        this->texture1 = other.texture1;
        this->texture2 = other.texture2;
        this->texture3 = other.texture3;
        this->VAO = other.VAO;
        this->shader = other.shader;
        this->drawElements = other.drawElements;
        this->drawn = other.drawn;

        return *this;
    }

    void Rigidbody::ApplyForce(const glm::vec3& force) {
        if (behavior != ObjectType::DYNAMIC)
            return;

        acceleration += force / mass;
    }

    void Rigidbody::PhysicsProcess(float deltaTime) {
        if (behavior != ObjectType::DYNAMIC)
            return;

        velocity += acceleration * deltaTime;
        SetPosition(position + velocity);

        acceleration = glm::vec3(0.0f);
    }

    void Rigidbody::SetPosition(glm::vec3 position_) {
        if (behavior == ObjectType::STATIC)
            return;

        position = position_;
        std::visit([&position_](auto& s) {
            s.position = position_;
        }, shape);
    }
    void Rigidbody::SetPosition(float x, float y, float z) {
        if (behavior == ObjectType::STATIC)
            return;

        position = glm::vec3(x, y, z);
        std::visit([&x, &y, &z](auto& s) {
            s.position = glm::vec3(x, y, z);
        }, shape);
    }
    glm::vec3 Rigidbody::GetPosition() {
        return position;
    }

    void Rigidbody::SetScale(glm::vec3 scale_) {
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
    void Rigidbody::SetScale(float x, float y, float z) {
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
    glm::vec3 Rigidbody::GetScale() {
        return scale;
    }
#pragma endregion

#pragma region Methods

bool operator==(const Rigidbody& A, const Rigidbody& B)
{
    bool same = true;

    if (A.acceleration != B.acceleration) same = false;
    if (A.velocity != B.velocity) same = false;
    if (A.mass != B.mass) same = false;
    //if (A.shape != B.shape) same = false;
    if (A.behavior != B.behavior) same = false;

    if (A.indexCount != B.indexCount) same = false;
    if (A.texture1 != B.texture1) same = false;
    if (A.texture2 != B.texture2) same = false;
    if (A.texture3 != B.texture3) same = false;
    if (A.VAO != B.VAO) same = false;
    if (A.shader != B.shader) same = false;
    if (A.drawElements != B.drawElements) same = false;
    if (A.drawn != B.drawn) same = false;

    return same;
}
