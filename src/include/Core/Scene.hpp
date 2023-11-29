#pragma once
#include "../glad/glad.h"
#include "Debug.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Physics/RigidBody.hpp"
#include "Physics/SpatialPhysics.hpp"
#include <vector>
#include <glm/glm.hpp>

struct SpatialObject
{
public:
    Mesh SO_mesh;
    Texture SO_texture;
    RigidBody SO_rigidbody;
    unsigned int SO_id;

    SpatialObject() 
    {
        SO_mesh = Mesh();
        SO_rigidbody = RigidBody();
        SO_texture = Texture();
        SO_id = -1;
    }
    SpatialObject(Mesh mesh, unsigned int id) : SO_mesh(mesh), SO_id(id) 
    {
        SO_rigidbody = RigidBody();
        SO_texture = Texture();
    }
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
    void DrawSingle(Shader *shader, glm::mat4 lightMat, glm::mat4 view, glm::mat4 proj, glm::vec3 camPos);
    void DrawSingleNoAssign(Shader *shader);
    void DeleteObjects();
    void Clear();

    //Scene saving does not support meshes that do not have a file
    void SaveScene(std::string location, std::string name);
};

void LoadScene(std::string location, std::string name, Scene& scene);


