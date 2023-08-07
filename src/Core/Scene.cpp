#include "../include/Core/Scene.hpp"

Scene::Scene()
{

}

Scene::~Scene()
{
    std::cout << "Scene got deleted" << std::endl;
    DeleteObjects();
}

void Scene::AddSpaceObject(Mesh mesh)
{
    unsigned int id = SpaceObjects.size();
    SpaceObjects.push_back(SpaceObject(mesh, id));
    SpaceObjects[id].SO_mesh.BufferGens();
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
    {
        drawNum = (float)(SpaceObjects.size() - (SpaceObjects.size() % stepSize)) / (float)stepSize;
        if(drawNum == 0)
        {
            drawNum = (float)SpaceObjects.size() / stepSize;
        }
    }
    else
        drawNum = 0;

    for (unsigned int i = 0; i < drawNum; i++)
    {
        for (unsigned int j = 0; j < stepSize; j++)
        {
            unsigned int indOffset = SpaceObjects[j + stepSize * i].SO_mesh.vertices.size();
            tmp_Vertices.insert(tmp_Vertices.end(), SpaceObjects[j + stepSize * i].SO_mesh.vertices.begin(), SpaceObjects[j + stepSize * i].SO_mesh.vertices.end());
            for (unsigned int g = 0; g < SpaceObjects[j + stepSize * i].SO_mesh.indices.size(); g++)
            {
                tmp_Indices.push_back(SpaceObjects[j + stepSize * i].SO_mesh.indices[g] + (indOffset * (j + stepSize * i)));
            }
             
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
            unsigned int index = (SpaceObjects.size() - (SpaceObjects.size() % stepSize)) + i;
            SpaceObjects[index].SO_mesh.DrawMesh();
        }
        
    }

}

void Scene::DrawSingle(Shader *shader)
{
    for (unsigned int i = 0; i < SpaceObjects.size(); i++)
    {
        shader->setMat4("model", SpaceObjects[i].SO_mesh.GetModelMat());
        shader->setVec4("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        SpaceObjects[i].SO_mesh.DrawMesh();
    }
    
}

void Scene::DeleteObjects()
{
    for (unsigned int i = 0; i < SpaceObjects.size(); i++)
    {
        SpaceObjects[i].SO_mesh.Delete();
    }
}