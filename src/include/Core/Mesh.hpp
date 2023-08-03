#pragma once
#include <glm/glm.hpp>
#include <vector>

class Mesh
{
private:

    unsigned int vao, vbo, ebo;

public:

    std::vector<float> vertexes;
    std::vector<unsigned int> indices;
    glm::vec3 position, rotation;
    float scale;

    Mesh(std::vector<float> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale);
    ~Mesh();
    void BufferGens();
    void DrawMesh();
    glm::mat4 GetModelMat();
};


Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation);

