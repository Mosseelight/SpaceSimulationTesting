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
    Shader SO_shader;
    unsigned int SO_id;

    SpaceObject(Mesh mesh, Shader shader, unsigned int id)
        : SO_mesh(mesh), SO_shader(shader), SO_id(id) {}
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

    void AddSpaceObject(Mesh mesh, ShaderLoc location);
    void DrawFull(unsigned int stepSize); // Will Draw every object 
};


