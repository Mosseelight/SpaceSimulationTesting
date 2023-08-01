#include "../include/Core/Mesh.hpp"
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

Mesh::Mesh(float vertexes[], unsigned int indices[])
{
    this->vertexes = vertexes;
    this->indices = indices;
}

Mesh::~Mesh()
{
    
}

void Mesh::BufferGens()
{
    glGenBuffers(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), vertexes, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(0);
}

void Mesh::DrawMesh()
{

}