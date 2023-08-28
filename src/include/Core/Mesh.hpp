#pragma once
#include "../glad/glad.h"
#include <SDL2/SDL.h>
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
    FileMesh,
    First = CubeMesh,
    Last = FileMesh
};

std::string GetMeshTypeName(MeshType type);

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv) 
    {
        this->position = position;
        this->normal = normal;
        this->uv = uv;
    }
    Vertex(const Vertex &other)
    {
        this->position = other.position;
        this->normal = other.normal;
        this->uv = other.uv;
    }
    ~Vertex() {}
};

class Mesh
{
private:

    unsigned int vao, vbo, ebo;
    bool BufferLock = false;
    std::vector<Vertex> CombineToVertex();

public:

    std::string modelLocation;
    std::vector<Vertex> vertexes;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uv;
    std::vector<unsigned int> indices;
    glm::vec3 position, rotation;
    float scale;

    Mesh();
    Mesh(const Mesh &other);
    Mesh(std::vector<Vertex> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale);
    Mesh(std::string modelLocation, std::vector<Vertex> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale);
    Mesh(std::vector<float> vertices, std::vector<float> normals, std::vector<float> uv, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale);
    void BufferGens();
    void ReGenBuffer();
    void DrawMesh();
    void Delete();
    glm::mat4 GetModelMat();
    void FixWindingOrder();
    void CreateSmoothNormals();
    void SubdivideTriangle();
    void Balloon(float delta, float speed, float percentage);
};


Mesh Create2DTriangle(glm::vec3 position, glm::vec3 rotation);
Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation);
Mesh CreateSphereMesh(glm::vec3 position, glm::vec3 rotation, unsigned int subdivideNum);


