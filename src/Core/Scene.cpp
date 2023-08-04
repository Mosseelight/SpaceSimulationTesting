#include "../include/Core/Scene.hpp"

Scene::Scene()
{

}

Scene::~Scene()
{
    
}

void Scene::AddSpaceObject(Mesh mesh, ShaderLoc location)
{
    unsigned int id = idList.size();
    SpaceObjects.push_back(SpaceObject(mesh, Shader(location), id));
    idList.push_back(id);
}

//dont know if 10 is good step num 
//test to figure out
void Scene::DrawFull(unsigned int stepSize)
{

    std::vector<float> tmp_Vertices;
    std::vector<unsigned int> tmp_Indices;
    unsigned int drawNum = 0;
    if(SpaceObjects.size() > stepSize)
        drawNum = stepSize / (SpaceObjects.size() - (SpaceObjects.size() % stepSize));
    else
        drawNum = 0;

    for (unsigned int i = 0; i < drawNum; i++)
    {
        unsigned int vao, vbo, ebo;
        for (unsigned int j = 0; j < stepSize; j++)
        {
            tmp_Vertices.insert(tmp_Vertices.end(), SpaceObjects[j + stepSize * i].SO_mesh.vertexes.begin(), SpaceObjects[j + stepSize * i].SO_mesh.vertexes.end());
            tmp_Indices.insert(tmp_Indices.end(), SpaceObjects[j + stepSize * i].SO_mesh.indices.begin(), SpaceObjects[j + stepSize * i].SO_mesh.indices.end());  
        }

        Mesh mesh = Mesh(tmp_Vertices, tmp_Indices, glm::vec3(0.0f), glm::vec3(0.0f), 1);
        mesh.BufferGens();
        mesh.DrawMesh();

        tmp_Vertices.clear();
        tmp_Indices.clear();
    }

    //if object total last digit is 1-9
    if(SpaceObjects.size() % stepSize != 0)
    {
        for (unsigned int i = 0; i < SpaceObjects.size() % stepSize; i++)
        {
            //draw that mesh
            //std::cout << (SpaceObjects.size() - (SpaceObjects.size() % stepSize)) + i << std::endl;
            unsigned int index = (SpaceObjects.size() - (SpaceObjects.size() % stepSize)) + i;
            SpaceObjects[index].SO_mesh.BufferGens();
            SpaceObjects[index].SO_mesh.DrawMesh();
        }
        
    }

}