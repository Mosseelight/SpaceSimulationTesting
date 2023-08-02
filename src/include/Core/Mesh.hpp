#pragma once
#include <glm/glm.hpp>

class Mesh
{
private:

    unsigned int vao, vbo, ebo;

public:

    float *vertexes;
    unsigned int *indices;
    glm::vec3 positon, rotation;
    float scale;
    glm::mat4 model;

    Mesh(float vertexs[], unsigned int indices[]);
    ~Mesh();
    void BufferGens();
    void DrawMesh();
    
};
