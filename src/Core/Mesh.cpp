#include "../include/Core/Mesh.hpp"
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(std::vector<float> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale)
{
    this->vertexes = vertexes;
    this->indices = indices;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
    BufferGens();
}

Mesh::~Mesh()
{
    
}

void Mesh::BufferGens()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(float), vertexes.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*) 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::DrawMesh()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

glm::mat4 Mesh::GetModelMat()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f,0.0f,0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f,0.0f,1.0f));
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    return model;
}