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


//--------------------Mesh Generation--------------------------

Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation)
{
    std::vector<float> vertices =
    {
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
        3, 2, 6,
        6, 7, 3
    };
    return Mesh(vertices, indices, position, rotation, 1.0f);
}

Mesh CreateSphereMesh(glm::vec3 position, glm::vec3 rotation)
{
    float t = (1.0 + sqrt(5.0)) / 2.0;

    std::vector<float> vertices = {
        -1,  t,  0,
        1,  t,  0,
        -1, -t,  0,
        1, -t,  0,
        0, -1,  t,
        0,  1,  t,
        0, -1, -t,
        0,  1, -t,
        t,  0, -1,
        t,  0,  1,
        -t,  0, -1,
        -t,  0,  1
    };

    std::vector<unsigned int> indices = {
        0, 11, 5,
        0, 5, 1,
        0, 1, 7,
        0, 7, 10,
        0, 10, 11,
        
        1, 5, 9,
        5, 11, 4,
        11, 10, 2,
        10, 7, 6,
        7, 1, 8,
        
        3, 9, 4,
        3, 4, 2,
        3, 2, 6,
        3, 6, 8,
        3, 8, 9,
        
        4, 9, 5,
        2, 4, 11,
        6, 2, 10,
        8, 6, 7,
        9, 8, 1
    };

    return Mesh(vertices, indices, position, rotation, 1.0f);
}