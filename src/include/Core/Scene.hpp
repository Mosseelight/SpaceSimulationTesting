#pragma once
#include "Mesh.hpp"
#include "Shader.hpp"
#include <vector>

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

    Scene();
    ~Scene();
};


