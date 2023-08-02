#pragma once
#include <string>

class Shader
{
private:

    unsigned int vertShaderU, fragShaderU;

public:

    unsigned int shader;
    Shader(char *vertCode, char *fragCode);
    ~Shader();
    template <typename T>
    void SetUniform(std::string, T value);
    
};
