#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Shader
{
public:
    unsigned int ID;

    Shader() = default;

    Shader(const char* vertexPath, const char* fragmentPath);

    void use();

    void setMat4(const std::string& name, glm::mat4 value) const;

    void setVec3(const std::string& name, glm::vec3 value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec2(const std::string& name, glm::vec2 value) const;
    void setVec2(const std::string& name, float x, float y) const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
protected:
    friend bool operator==(const Shader& A, const Shader& B);
    friend bool operator!=(const Shader& A, const Shader& B);
};

inline bool operator==(const Shader& A, const Shader& B)
{
    if (A.ID == B.ID) return true;

    return false;
}

inline bool operator!=(const Shader& A, const Shader& B)
{
    if (A.ID == B.ID) return false;

    return true;
}