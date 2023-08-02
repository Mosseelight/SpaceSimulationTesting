#include "../include/Core/Mesh.hpp"
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

Mesh::Mesh(float vertexes[], unsigned int indices[], unsigned int vertexCount, unsigned int indiceCount)
{
    this->vertexes = vertexes;
    this->indices = indices;
    this->vertexCount = vertexCount;
    this->indiceCount = indiceCount;
    BufferGens();
}

Mesh::~Mesh()
{
    
}

void Mesh::BufferGens()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount, vertexes, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceCount, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::DrawMesh()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indiceCount, GL_UNSIGNED_INT, 0);
}