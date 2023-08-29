#include "../include/Core/Scene.hpp"

unsigned int SpatialObject::GetSizeUsage()
{
    unsigned int total = 0;
    total += sizeof(SpatialObject); // get variable sizes

    //mesh data sizes
    total += sizeof(Vertex) * SO_mesh.vertexes.size();
    total += sizeof(float) * SO_mesh.vertices.size();
    total += sizeof(float) * SO_mesh.normals.size();
    total += sizeof(float) * SO_mesh.uv.size();
    total += sizeof(unsigned int) * SO_mesh.indices.size();
    return total;
}

Scene::Scene()
{

}

Scene::~Scene()
{
    DebugLog("Scene got deleted");
    DeleteObjects();
}

void Scene::AddSpatialObject(Mesh mesh)
{
    unsigned int id = SpatialObjects.size();
    SpatialObjects.push_back(SpatialObject(mesh, id));
    SpatialObjects[id].SO_mesh.BufferGens();
    idList.push_back(id);

    if(SpatialObjects[id].GetSizeUsage() / 1024 < 1000)
        DebugLog("Added spatial object mesh: " + mesh.modelLocation + ", size: " + std::to_string(SpatialObjects[id].GetSizeUsage() / 1024.0f) + "kb");
    else if(SpatialObjects[id].GetSizeUsage() / 1024 / 1024 < 1000)
        DebugLog("Added spatial object mesh: " + mesh.modelLocation + ", size: " + std::to_string(SpatialObjects[id].GetSizeUsage() / 1024.0f / 1024.0f) + "mb");
    else
        DebugLog("Added spatial object mesh: " + mesh.modelLocation + ", size: " + std::to_string(SpatialObjects[id].GetSizeUsage() / 1024.0f / 1024.0f / 1024.0f) + "gb");
}

//dont know if 10 is good step num 
//test to figure out
/*void Scene::DrawFull(unsigned int stepSize)
{

    std::vector<float> tmp_Vertices;
    std::vector<unsigned int> tmp_Indices;
    unsigned int drawNum = 0;
    if(SpatialObjects.size() > stepSize)
    {
        drawNum = (float)(SpatialObjects.size() - (SpatialObjects.size() % stepSize)) / (float)stepSize;
        if(drawNum == 0)
        {
            drawNum = (float)SpatialObjects.size() / stepSize;
        }
    }
    else
        drawNum = 0;

    for (unsigned int i = 0; i < drawNum; i++)
    {
        for (unsigned int j = 0; j < stepSize; j++)
        {
            unsigned int indOffset = SpatialObjects[j + stepSize * i].SO_mesh.vertices.size();
            tmp_Vertices.insert(tmp_Vertices.end(), SpatialObjects[j + stepSize * i].SO_mesh.vertices.begin(), SpatialObjects[j + stepSize * i].SO_mesh.vertices.end());
            for (unsigned int g = 0; g < SpatialObjects[j + stepSize * i].SO_mesh.indices.size(); g++)
            {
                tmp_Indices.push_back(SpatialObjects[j + stepSize * i].SO_mesh.indices[g] + (indOffset * (j + stepSize * i)));
            }
             
        }

        Mesh mesh = Mesh(tmp_Vertices, tmp_Indices, glm::vec3(0.0f), glm::vec3(0.0f), 1);
        mesh.BufferGens();
        mesh.DrawMesh();

        tmp_Vertices.clear();
        tmp_Indices.clear();
    }

    //if object total last digit is 1-9
    if(SpatialObjects.size() % stepSize != 0)
    {
        for (unsigned int i = 0; i < SpatialObjects.size() % stepSize; i++)
        {
            //draw that mesh
            unsigned int index = (SpatialObjects.size() - (SpatialObjects.size() % stepSize)) + i;
            SpatialObjects[index].SO_mesh.DrawMesh();
        }
        
    }

}*/

void Scene::DrawSingle(Shader *shader, glm::mat4 view, glm::mat4 proj)
{
    for (unsigned int i = 0; i < SpatialObjects.size(); i++)
    {
        shader->setMat4("model", SpatialObjects[i].SO_mesh.GetModelMat());
        shader->setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
        shader->setMat4("proj", proj);
        shader->setMat4("view", view);
        shader->setMat4("normalMat", glm::transpose(glm::inverse(SpatialObjects[i].SO_mesh.GetModelMat())));
        SpatialObjects[i].SO_mesh.DrawMesh();
    }
    
}

void Scene::DeleteObjects()
{
    for (unsigned int i = 0; i < SpatialObjects.size(); i++)
    {
        SpatialObjects[i].SO_mesh.Delete();
        DebugLog("Deleted Space Object id: " + SpatialObjects[i].SO_id);
    }
}

/*Scene Save format
(Scene name)
(number of SpatialObjects)
Spatialobject/(object number)
(Mesh location)
(Mesh position.x)/(mesh position.y)/(mesh position.z)
(Mesh rotation.x)/(mesh rotation.y)/(mesh rotation.z)
(mesh scale)

*/
void Scene::SaveScene(std::string location, std::string name)
{

}

void Scene::LoadScene(std::string location, std::string name)
{

}