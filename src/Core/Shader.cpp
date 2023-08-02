#include "../include/Core/Shader.hpp"
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <typeinfo>
#include <cstring>

Shader::Shader(std::string vertCode, std::string fragCode)
{
    const char *vertCodearr = vertCode.c_str();
    const char *fragCodearr = fragCode.c_str();
    int success;
    char infoLog[512];
    vertShaderU = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderU, 1, &vertCodearr, NULL);
    glCompileShader(vertShaderU);
    glGetShaderiv(vertShaderU, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertShaderU, 512, NULL, infoLog);
        std::cout << "Shader type Vertex Failed Compile\n" <<
        infoLog << std::endl;
    }

    fragShaderU = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderU, 1, &fragCodearr, NULL);
    glCompileShader(fragShaderU);
    glGetShaderiv(fragShaderU, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragShaderU, 512, NULL, infoLog);
        std::cout << "Shader type Fragment Failed Compile\n" <<
        infoLog << std::endl;
    }

    shader = glCreateProgram();

    glAttachShader(shader, vertShaderU);
    glAttachShader(shader, fragShaderU);
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader Failed Linking\n" <<
        infoLog << std::endl;
    }

    //maybe will create problem
    glDetachShader(shader, vertShaderU);
    glDetachShader(shader, fragShaderU);


    glDeleteShader(vertShaderU);
    glDeleteShader(fragShaderU);
}

Shader::~Shader()
{
    glDeleteProgram(shader);
}

void Shader::setBool(std::string name, bool value)
{         
    glUniform1i(glGetUniformLocation(shader, name.c_str()), (int)value); 
}
void Shader::setInt(std::string name, int value)
{ 
    glUniform1i(glGetUniformLocation(shader, name.c_str()), value); 
}
void Shader::setFloat(std::string name, float value)
{ 
    glUniform1f(glGetUniformLocation(shader, name.c_str()), value); 
}
void Shader::setVec2(std::string name, glm::vec2 value)
{ 
    glUniform2fv(glGetUniformLocation(shader, name.c_str()), 1, &value[0]); 
}
void Shader::setVec2(std::string name, float x, float y)
{ 
    glUniform2f(glGetUniformLocation(shader, name.c_str()), x, y); 
}
void Shader::setVec3(std::string name, glm::vec3 value)
{ 
    glUniform3fv(glGetUniformLocation(shader, name.c_str()), 1, &value[0]); 
}
void Shader::setVec3(std::string name, float x, float y, float z)
{ 
    glUniform3f(glGetUniformLocation(shader, name.c_str()), x, y, z); 
}
void Shader::setVec4(std::string name, glm::vec4 value)
{ 
    glUniform4fv(glGetUniformLocation(shader, name.c_str()), 1, &value[0]); 
}
void Shader::setVec4(std::string name, float x, float y, float z, float w)
{ 
    glUniform4f(glGetUniformLocation(shader, name.c_str()), x, y, z, w); 
}
void Shader::setMat4(std::string name, glm::mat4 mat)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}