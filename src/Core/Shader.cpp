#include "../include/Core/Shader.hpp"
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

Shader::Shader(char *vertCode, char *fragCode)
{
    int success;
    char infoLog[512];
    vertShaderU = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderU, 1, &vertCode, NULL);
    glCompileShader(vertShaderU);
    glGetShaderiv(vertShaderU, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertShaderU, 512, NULL, infoLog);
        std::cout << "Shader type Vertex Failed Compile\n" <<
        infoLog << std::endl;
    }

    fragShaderU = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderU, 1, &fragCode, NULL);
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

template <typename T>
void Shader::SetUniform(std::string name, T value)
{

}
