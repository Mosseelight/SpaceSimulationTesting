#pragma once
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <typeinfo>
#include <cstring>
#include <string>

struct ShaderLoc
{
    std::string vertCode;
    std::string fragCode;

    ShaderLoc(std::string vertloc, std::string fragloc) 
        : vertCode(vertloc), fragCode(fragloc) {}
    ~ShaderLoc() {}
};

class Shader
{
private:

    unsigned int vertShaderU, fragShaderU;

public:

    unsigned int shader;
    Shader();
    ~Shader();
    void CompileShader(ShaderLoc location);
    void setBool(std::string name, bool value);
    void setInt(std::string name, int value);
    void setFloat(std::string name, float value);
    void setVec2(std::string name, glm::vec2 value);
    void setVec2(std::string name, float x, float y);
    void setVec3(std::string name, glm::vec3 value);
    void setVec3(std::string name, float x, float y, float z);
    void setVec4(std::string name, glm::vec4 value);
    void setVec4(std::string name, float x, float y, float z, float w);
    void setMat4(std::string name, glm::mat4 mat);
    
};
