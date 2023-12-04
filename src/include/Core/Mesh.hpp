#pragma once
#include "../glad/glad.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "Globals.hpp"
#include "Debug.hpp"

enum MeshType
{
    CubeMesh,
    IcoSphereMesh,
    SpikerMesh,
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

public:

    std::string modelLocation;
    std::vector<Vertex> vertexes;
    std::vector<unsigned int> indices;
    glm::vec3 position, rotation;
    float scale;
    glm::mat4 modelMatrix;
    glm::mat4 rotMatrix;

    Mesh();
    Mesh(const Mesh &other);
    Mesh(std::vector<Vertex> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale);
    Mesh(std::string modelLocation, std::vector<Vertex> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale);
    void BufferGens();
    void ReGenBuffer();
    void DrawMesh();
    void Delete();
    void CreateModelMat();
    void CreateRotationMat();
    void FixWindingOrder();
    void CreateSmoothNormals();
    void SubdivideTriangle();
    void Balloon(float delta, float speed, float percentage);
};

Mesh LoadModel(glm::vec3 position, glm::vec3 rotation, std::string location);

Mesh Create2DTriangle(glm::vec3 position, glm::vec3 rotation);
Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation);
Mesh CreateSphereMesh(glm::vec3 position, glm::vec3 rotation, unsigned int subdivideNum);
Mesh CreateSpikerMesh(glm::vec3 position, glm::vec3 rotation, float size, unsigned int subdivideNum);


