#pragma once
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include "Mesh.hpp"
#include "Shader.hpp"
#include <vector>
#include <glm/glm.hpp>

struct SpaceObject
{
public:
    Mesh SO_mesh;
    unsigned int SO_id;

    SpaceObject(Mesh mesh, unsigned int id)
        : SO_mesh(mesh), SO_id(id) {}
    ~SpaceObject() {}
};

class Scene
{
private:
    
public:

    std::vector<SpaceObject> SpaceObjects;
    std::vector<unsigned int> idList;

    Scene();
    ~Scene();

    void AddSpaceObject(Mesh mesh);
    void DrawFull(unsigned int stepSize); // Will Draw every object 
    void DrawSingle(Shader *shader); 
};


