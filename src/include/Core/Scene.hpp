#pragma once
#include "../glad/glad.h"
#include "Debug.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <vector>
#include <glm/glm.hpp>

struct SpatialObject
{
public:
    Mesh SO_mesh;
    Texture SO_texture;
    unsigned int SO_id;

    SpatialObject() {}
    SpatialObject(Mesh mesh, unsigned int id)
        : SO_mesh(mesh), SO_id(id) {}
    ~SpatialObject() {}

    unsigned int GetSizeUsage();
};

class Scene
{
private:
    
public:

    std::vector<SpatialObject> SpatialObjects;
    std::vector<unsigned int> idList;

    Scene();
    Scene(const Scene &other);
    ~Scene();

    void AddSpatialObject(Mesh mesh);
    void DrawFull(unsigned int stepSize); // Will Draw every object 
    void DrawSingle(Shader *shader, glm::mat4 view, glm::mat4 proj); 
    void DeleteObjects();
    void Clear();

    //Scene saving does not support meshes that do not have a file
    void SaveScene(std::string location, std::string name);
};

void LoadScene(std::string location, std::string name, Scene& scene);


