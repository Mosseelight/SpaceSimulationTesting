#pragma once
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include "Globals.hpp"

enum MeshType
{
    CubeMesh,
    IcoSphereMesh,
    TriangleMesh,
    First = CubeMesh,
    Last = TriangleMesh
};

std::string GetMeshTypeName(MeshType type);

class Mesh
{
private:

    unsigned int vao, vbo, ebo;

public:

    std::vector<float> vertexes;
    std::vector<unsigned int> indices;
    glm::vec3 position, rotation;
    float scale;

    Mesh();
    Mesh(std::vector<float> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale);
    ~Mesh();
    void BufferGens();
    void ReGenBuffer();
    void DrawMesh();
    glm::mat4 GetModelMat();
    void SubdivideTriangle();
};


Mesh Create2DTriangle(glm::vec3 position, glm::vec3 rotation);
Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation);
Mesh CreateSphereMesh(glm::vec3 position, glm::vec3 rotation, unsigned int subdivideNum);


