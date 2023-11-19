#include "../include/Core/Scene.hpp"

unsigned int SpatialObject::GetSizeUsage()
{
    unsigned int total = 0;
    total += sizeof(SpatialObject); // get variable sizes

    //mesh data sizes
    total += sizeof(Vertex) * SO_mesh.vertexes.size();
    total += sizeof(unsigned int) * SO_mesh.indices.size();
    return total;
}

Scene::Scene()
{

}

Scene::Scene(const Scene &other)
{
    this->SpatialObjects = other.SpatialObjects;
    this->idList = other.idList;
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
    SpatialObjects[id].SO_rigidbody.position = mesh.position;
    SpatialObjects[id].SO_rigidbody.rotation = mesh.rotation;
    idList.push_back(id);

    SpatialObjects[id].SO_mesh.CreateModelMat();
    SpatialObjects[id].SO_mesh.CreateRotationMat();
    SpatialObjects[id].SO_rigidbody.boundbox.ConstructBoundingBox(SpatialObjects[id].SO_mesh);
    SpatialObjects[id].SO_rigidbody.oriBoundBox.ConstructOriBoundingBox(SpatialObjects[id].SO_mesh);

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

void Scene::DrawSingle(Shader *shader, glm::mat4 lightMat, glm::mat4 view, glm::mat4 proj, glm::vec3 camPos)
{
    for (unsigned int i = 0; i < SpatialObjects.size(); i++)
    {
        shader->setMat4("model", SpatialObjects[i].SO_mesh.modelMatrix);
        shader->setMat4("lightSpaceMatrix", lightMat);
        shader->setVec3("viewPos", camPos);
        shader->setMat4("projection", proj);
        shader->setMat4("view", view);
        SpatialObjects[i].SO_mesh.DrawMesh();
    }
}

void Scene::DrawSingleNoAssign(Shader *shader)
{
    for (unsigned int i = 0; i < SpatialObjects.size(); i++)
    {
        shader->setMat4("model", SpatialObjects[i].SO_mesh.modelMatrix);
        SpatialObjects[i].SO_mesh.DrawMesh();
    }
}

void Scene::DeleteObjects()
{
    for (unsigned int i = 0; i < SpatialObjects.size(); i++)
    {
        SpatialObjects[i].SO_mesh.Delete();
        DebugLog("Deleted Space Object id: " + std::to_string(SpatialObjects[i].SO_id));
    }
}

void Scene::Clear()
{
    for (unsigned int i = 0; i < SpatialObjects.size(); i++)
    {
        SpatialObjects[i].SO_mesh.Delete();
        SpatialObjects[i].SO_texture.Delete();
        DebugLog("Deleted Space Object id: " + std::to_string(SpatialObjects[i].SO_id));
    }
    SpatialObjects.clear();
    idList.clear();
}

/*Scene Save format

S (Scene name)
SN (number of SpatialObjects)
SO (object number)
ML (Mesh location)
MP (Mesh position.x)/(mesh position.y)/(mesh position.z)
MR (Mesh rotation.x)/(mesh rotation.y)/(mesh rotation.z)
MS (mesh scale)
TL (Texture location)
RS (isStatic)
RM (Mass)
RV (Velocity.x)/(Velocity.y)/(Velocity.z)
RA (Acceleration.x)/(Acceleration.y)/(Acceleration.z)
RVR (RotVelocity.x)/(RotVelocity.y)/(RotVelocity.z)
RAR (RotAcceleration.x)/(RotAcceleration.y)/(RotAcceleration.z)
RF (TotalForce.x)/(TotalForce.y)/(TotalForce.z)
RFR (RotTotalForce.x)/(RotTotalForce.y)/(RotTotalForce.z)

*/
void Scene::SaveScene(std::string location, std::string name)
{
    if(!FileExist(location + name))
        CreateFile(location, name);
    else
    {
        std::ofstream stream;
        stream.open(location + name, std::ofstream::out | std::ofstream::trunc);
        stream.close();
    }
    size_t endExt = name.find_last_of('.');
    std::string newName = name.substr(0, endExt);
    std::string info =
        "#Scene File\n"
        "#Scene and Object Layout\n"
        "\n"
        "#S (Scene name)\n"
        "#SN (number of SpatialObjects)\n"
        "#SO (object number)\n"
        "#ML (Mesh location)\n"
        "#MP (Mesh position.x)/(mesh position.y)/(mesh position.z)\n"
        "#MR (Mesh rotation.x)/(mesh rotation.y)/(mesh rotation.z)\n"
        "#MS (mesh scale)\n"
        "#TL (Texture location)\n"
        "#RS (isStatic)\n"
        "#RM (Mass)\n"
        "#RV (Velocity.x)/(Velocity.y)/(Velocity.z)\n"
        "#RA (Acceleration.x)/(Acceleration.y)/(Acceleration.z)\n"
        "#RVR (RotVelocity.x)/(RotVelocity.y)/(RotVelocity.z)\n"
        "#RAR (RotAcceleration.x)/(RotAcceleration.y)/(RotAcceleration.z)\n"
        "#RF (TotalForce.x)/(TotalForce.y)/(TotalForce.z)\n"
        "#RFR (RotTotalForce.x)/(RotTotalForce.y)/(RotTotalForce.z)\n";

    WriteFile(location + name, info);
    WriteFile(location + name, "S " + newName);
    WriteFile(location + name, "SN " + std::to_string(SpatialObjects.size()));

    for (unsigned int i = 0; i < SpatialObjects.size(); i++)
    {
        WriteFile(location + name, "SO " + std::to_string(SpatialObjects[i].SO_id));
        WriteFile(location + name, "ML " + SpatialObjects[i].SO_mesh.modelLocation);
        WriteFile(location + name, "MP " + std::to_string(SpatialObjects[i].SO_mesh.position.x) + "/" + std::to_string(SpatialObjects[i].SO_mesh.position.y) + "/" + std::to_string(SpatialObjects[i].SO_mesh.position.z));
        WriteFile(location + name, "MR " + std::to_string(SpatialObjects[i].SO_mesh.rotation.x) + "/" + std::to_string(SpatialObjects[i].SO_mesh.rotation.y) + "/" + std::to_string(SpatialObjects[i].SO_mesh.rotation.z));
        WriteFile(location + name, "MS " + std::to_string(SpatialObjects[i].SO_mesh.scale));
        WriteFile(location + name, "TL " + SpatialObjects[i].SO_texture.textLocation);
        WriteFile(location + name, "RS " + std::to_string(SpatialObjects[i].SO_rigidbody.isStatic));
        WriteFile(location + name, "RM " + std::to_string(SpatialObjects[i].SO_rigidbody.mass));
        WriteFile(location + name, "RV " + std::to_string(SpatialObjects[i].SO_rigidbody.velocity.x) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.velocity.y) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.velocity.z));
        WriteFile(location + name, "RA " + std::to_string(SpatialObjects[i].SO_rigidbody.acceleration.x) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.acceleration.y) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.acceleration.z));
        WriteFile(location + name, "RVR " + std::to_string(SpatialObjects[i].SO_rigidbody.rotVelocity.x) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.rotVelocity.y) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.rotVelocity.z));
        WriteFile(location + name, "RAR " + std::to_string(SpatialObjects[i].SO_rigidbody.rotAcceleration.x) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.rotAcceleration.y) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.rotAcceleration.z));
        WriteFile(location + name, "RF " + std::to_string(SpatialObjects[i].SO_rigidbody.totalForce.x) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.totalForce.y) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.totalForce.z));
        WriteFile(location + name, "RFR " + std::to_string(SpatialObjects[i].SO_rigidbody.totalRotation.x) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.totalRotation.y) + "/" + std::to_string(SpatialObjects[i].SO_rigidbody.totalRotation.z));
    }
    
}

void LoadScene(std::string location, std::string name, Scene& scene)
{
    if(!FileExist(location + name))
    {
        DebugLog("Scene not found");
        return;
    }
    DebugLog("Loading scene " + name);
    scene.Clear();
    std::ifstream file(location + name);
    std::string line;
    unsigned int size = 0;
    unsigned int id = 0;
    SpatialObject tempObject;

    if(std::getline(file, line))
    {
        if(line != "#Scene File")
        {
            DebugLog("Uncorrect Scene Format Missing (#Scene File)");
            return;
        }
    }
    
    while (std::getline(file, line))
    {
        if(line[0] == 'S' && line[1] == 'N')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            std::istringstream data(line);
            data >> size;
        }
        if(line[0] == 'S' && line[1] == 'O')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            std::istringstream data(line);
            unsigned int id;
            data >> id;
            tempObject.SO_id = id;
        }
        if(line[0] == 'M' && line[1] == 'L')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            tempObject.SO_mesh.modelLocation = line;
        }
        if(line[0] == 'M' && line[1] == 'P')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            tempObject.SO_mesh.position = glm::vec3(x,y,z);
        }
        if(line[0] == 'M' && line[1] == 'R')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            tempObject.SO_mesh.rotation = glm::vec3(x,y,z);
        }
        if(line[0] == 'M' && line[1] == 'S')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            std::istringstream data(line);
            float s;
            data >> s;
            tempObject.SO_mesh.scale = s;
        }
        if(line[0] == 'T' && line[1] == 'L')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            tempObject.SO_texture.textLocation = line;
            id = scene.SpatialObjects.size();
            scene.AddSpatialObject(LoadModel(tempObject.SO_mesh.position, tempObject.SO_mesh.rotation, tempObject.SO_mesh.modelLocation));
        }
        if(line[0] == 'R' && line[1] == 'S')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            std::istringstream data(line);
            unsigned int val;
            data >> val;
            scene.SpatialObjects[id].SO_rigidbody.isStatic = val;
        }
        if(line[0] == 'R' && line[1] == 'M')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            std::istringstream data(line);
            float mass;
            data >> mass;
            scene.SpatialObjects[id].SO_rigidbody.mass = mass;
        }
        if(line[0] == 'R' && line[1] == 'V' && line[2] != 'R')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            scene.SpatialObjects[id].SO_rigidbody.velocity = glm::vec3(x,y,z);
        }
        if(line[0] == 'R' && line[1] == 'A' && line[2] != 'R')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            scene.SpatialObjects[id].SO_rigidbody.acceleration = glm::vec3(x,y,z);
        }
        if(line[0] == 'R' && line[1] == 'V' && line[2] == 'R')
        {
            line.erase(line.begin(), line.begin() + 3);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            scene.SpatialObjects[id].SO_rigidbody.rotVelocity = glm::vec3(x,y,z);
        }
        if(line[0] == 'R' && line[1] == 'A' && line[2] == 'R')
        {
            line.erase(line.begin(), line.begin() + 3);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            scene.SpatialObjects[id].SO_rigidbody.rotAcceleration = glm::vec3(x,y,z);
        }
        if(line[0] == 'R' && line[1] == 'F' && line[2] != 'R')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            scene.SpatialObjects[id].SO_rigidbody.totalForce = glm::vec3(x,y,z);
        }
        if(line[0] == 'R' && line[1] == 'F' && line[2] == 'R')
        {
            line.erase(line.begin(), line.begin() + 3);
            line = stringRemove(line, " ", "");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            scene.SpatialObjects[id].SO_rigidbody.totalRotation = glm::vec3(x,y,z);
        }
    }
    
    file.close();
}