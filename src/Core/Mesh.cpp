#include "../include/Core/Mesh.hpp"

std::string GetMeshTypeName(MeshType type)
{
    switch (type)
    {
    case CubeMesh:
        return "CubeMesh";
        break;
    case IcoSphereMesh:
        return "IcoSphereMesh";
        break;
    case TriangleMesh:
        return "TriangleMesh";
        break;
    case FileMesh:
        return "FileMesh";
        break;
    }
    return "No Mesh";
}

Mesh::Mesh()
{

}

Mesh::Mesh(const Mesh &other)
{
    this->modelLocation = other.modelLocation;
    this->vao = other.vao;
    this->vbo = other.vbo;
    this->ebo = other.ebo;
    this->vertexes = other.vertexes;
    this->indices = other.indices;
    this->position = other.position;
    this->rotation = other.rotation;
    this->scale = other.scale;
}

Mesh::Mesh(std::vector<Vertex> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale)
{
    this->vertexes = vertexes;
    this->indices = indices;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

Mesh::Mesh(std::string modelLocation, std::vector<Vertex> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale)
{
    this->modelLocation = modelLocation;
    this->vertexes = vertexes;
    this->indices = indices;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

void Mesh::Delete()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}

void Mesh::BufferGens()
{
    BufferLock = true;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
  
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(Vertex), vertexes.data(), GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
    BufferLock = false;
}

void Mesh::ReGenBuffer()
{
    BufferLock = true;
    Delete();
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
  
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(Vertex), vertexes.data(), GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
    BufferLock = false;
}

void Mesh::DrawMesh()
{
    if(!BufferLock)
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        DrawCallCount++;
    }
}

glm::mat4 Mesh::GetModelMat()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x * M_PIf/180.0f, glm::vec3(1.0f,0.0f,0.0f));
    model = glm::rotate(model, rotation.y * M_PIf/180.0f, glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, rotation.z * M_PIf/180.0f, glm::vec3(0.0f,0.0f,1.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    return model;
}

glm::mat4 Mesh::GetRotationMat()
{
    glm::mat4 mat = glm::mat4(1.0f);
    mat = glm::rotate(mat, rotation.x * M_PIf/180.0f, glm::vec3(1.0f,0.0f,0.0f));
    mat = glm::rotate(mat, rotation.y * M_PIf/180.0f, glm::vec3(0.0f,1.0f,0.0f));
    mat = glm::rotate(mat, rotation.z * M_PIf/180.0f, glm::vec3(0.0f,0.0f,1.0f));
    return mat;
}

void Mesh::FixWindingOrder()
{
    if(vertexes.size() == 0)
    {

    }
    else
    {
        glm::vec3 vertex1, vertex2, vertex3;
        for (unsigned int i = 0; i < vertexes.size() / 3; i++)
        {
            vertex1 = vertexes[i * 3].position; vertex2 = vertexes[i * 3 + 1].position; vertex1 = vertexes[i * 3 + 2].position;
            glm::vec3 u = vertex2 - vertex1;
            glm::vec3 v = vertex3 - vertex1;

            glm::vec3 normal = glm::cross(u,v);
            if(normal.z < 0)
            {
                unsigned int ind1, ind2, ind3;
                ind1 = indices[i * 3];
                ind2 = indices[i * 3 + 1];
                ind3 = indices[i * 3 + 2];
                indices[i * 3] = ind3;
                indices[i * 3 + 1] = ind2;
                indices[i * 3 + 2] = ind1;
            }
        }
        
    }
}

//---------------------------
//Needs fixing
//---------------------------
void Mesh::CreateSmoothNormals()
{
    for (unsigned int v = 0; v < vertexes.size(); v++)
    {
        glm::vec3 normal;
        for (unsigned int i = 0; i < indices.size(); i += 3)
        {
            unsigned int a,b,c;
            a = indices[i];
            b = indices[i + 1];
            c = indices[i + 2];
            if(vertexes[v].position == vertexes[a].position)
            {
                glm::vec3 u = vertexes[b].position - vertexes[a].position;
                glm::vec3 v = vertexes[c].position - vertexes[a].position;
                glm::vec3 tmpnormal = glm::normalize(glm::cross(u,v));
                normal += tmpnormal;
            }
            if(vertexes[v].position == vertexes[b].position)
            {
                glm::vec3 u = vertexes[a].position - vertexes[b].position;
                glm::vec3 v = vertexes[c].position - vertexes[b].position;
                glm::vec3 tmpnormal = glm::normalize(glm::cross(u,v));
                normal += tmpnormal;
            }
            if(vertexes[v].position == vertexes[c].position)
            {
                glm::vec3 u = vertexes[b].position - vertexes[c].position;
                glm::vec3 v = vertexes[a].position - vertexes[c].position;
                glm::vec3 tmpnormal = glm::normalize(glm::cross(u,v));
                normal += tmpnormal;
            }
        }
        vertexes[v].normal = glm::normalize(normal);
    }
}
 
void Mesh::SubdivideTriangle()
{
    std::vector<unsigned int> newIndices;
    std::vector<Vertex> newVerts;
    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        //ia is the row in the vertices array
        unsigned int ia = indices[i]; 
        unsigned int ib = indices[i + 1];
        unsigned int ic = indices[i + 2]; 
        Vertex a = vertexes[ia];
        Vertex b = vertexes[ib];
        Vertex c = vertexes[ic];
        glm::vec3 ab = (a.position + b.position) * 0.5f;
        glm::vec3 bc = (b.position + c.position) * 0.5f;
        glm::vec3 ca = (c.position + a.position) * 0.5f;
        glm::vec3 u = bc - ab;
        glm::vec3 v = ca - ab;
        glm::vec3 normal = glm::normalize(glm::cross(u,v));
        ia = newVerts.size();
        newVerts.push_back(a);
        ib = newVerts.size();
        newVerts.push_back(b);
        ic = newVerts.size();
        newVerts.push_back(c);
        unsigned int iab = newVerts.size();
        newVerts.push_back(Vertex(ab, normal, glm::vec2(0)));
        unsigned int ibc = newVerts.size(); 
        newVerts.push_back(Vertex(bc, normal, glm::vec2(0)));
        unsigned int ica = newVerts.size(); 
        newVerts.push_back(Vertex(ca, normal, glm::vec2(0)));
        newIndices.push_back(ia); newIndices.push_back(iab); newIndices.push_back(ica);
        newIndices.push_back(ib); newIndices.push_back(ibc); newIndices.push_back(iab);
        newIndices.push_back(ic); newIndices.push_back(ica); newIndices.push_back(ibc);
        newIndices.push_back(iab); newIndices.push_back(ibc); newIndices.push_back(ica);
    }
    indices.clear();
    indices = newIndices;
    vertexes.clear();
    vertexes = newVerts;
}

void Mesh::Balloon(float delta = 0.0f, float speed = 0.0f, float percentage = 0.0f)
{
    for (unsigned int i = 0; i < vertexes.size(); i++)
    {
        glm::vec3 finPos = glm::normalize(vertexes[i].position);
        if(percentage == 0)
            vertexes[i].position = vertexes[i].position * (1.0f - ((SDL_GetTicks64() / 1000.0f) * delta * speed)) + finPos * ((SDL_GetTicks64() / 1000.0f) * delta * speed);
        else
            vertexes[i].position = vertexes[i].position * (1.0f - percentage) + finPos * percentage;
    }
    
}

Mesh LoadModel(glm::vec3 position, glm::vec3 rotation, std::string location)
{
    std::ifstream file(location);
    if(!file)
    {
        DebugLog("Location for Mesh not found " + location);
        switch (static_cast<MeshType>(std::stoi(location)))
        {
        case CubeMesh:
            return CreateCubeMesh(position, rotation);
            break;
        case IcoSphereMesh:
            return CreateSphereMesh(position, rotation, 4);
            break;
        case TriangleMesh:
            return Create2DTriangle(position, rotation);
            break;
        }
    }

    Mesh mesh;
    mesh.position = position;
    mesh.rotation = rotation;
    mesh.scale = 1;

    std::string line;
    std::vector<Vertex> vertexes;
    std::vector<glm::vec2> tmpUV;
    std::vector<glm::vec3> tmpNormal;
    std::vector<glm::vec3> tmpVertice;
    std::vector<unsigned int> tmpInd, tmpUVInd, tmpNormalInd;
    Vertex vertex(glm::vec3(0), glm::vec3(0), glm::vec2(0));
    while(std::getline(file, line))
    {
        if(line[0] == 'v' && line[1] != 't' && line[1] != 'n')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            tmpVertice.push_back(glm::vec3(x,y,z));
        }
        else if(line[0] == 'v' && line[1] == 't' && line[1] != 'n')
        {
            line.erase(line.begin(), line.begin() + 3);
            line = stringRemove(line, " ", " ");
            std::istringstream data(line);
            float x, y;
            data >> x;
            data >> y;
            tmpUV.push_back(glm::vec2(x, y));
        }
        else if(line[0] == 'v' && line[1] != 't' && line[1] == 'n')
        {
            line.erase(line.begin(), line.begin() + 3);
            line = stringRemove(line, " ", " ");
            std::istringstream data(line);
            float x, y, z;
            data >> x;
            data >> y;
            data >> z;
            tmpNormal.push_back(glm::vec3(x, y, z));
        }
        else if(line[0] == 'f')
        {
            line.erase(line.begin(), line.begin() + 2);
            line = stringRemove(line, " ", " ");
            line = stringRemove(line, "/", " ");
            std::istringstream data(line);
            unsigned int ind, uvind, norind;
            data >> ind;
            data >> uvind;
            data >> norind;
            tmpInd.push_back(ind); tmpUVInd.push_back(uvind); tmpNormalInd.push_back(norind);
            data >> ind;
            data >> uvind;
            data >> norind;
            tmpInd.push_back(ind); tmpUVInd.push_back(uvind); tmpNormalInd.push_back(norind);
            data >> ind;
            data >> uvind;
            data >> norind;
            tmpInd.push_back(ind); tmpUVInd.push_back(uvind); tmpNormalInd.push_back(norind);
        }
    }
    for (unsigned int i = 0; i < tmpInd.size(); i++)
    {
        unsigned int indUv = tmpUVInd[i];
        unsigned int indNor = tmpNormalInd[i];
        unsigned int indVert = tmpInd[i];
        mesh.indices.push_back(i);
        vertexes.push_back(vertex);
        vertexes[i].uv = tmpUV[indUv - 1];
        vertexes[i].normal = tmpNormal[indNor - 1]; 
        vertexes[i].position = tmpVertice[indVert - 1];
    }
    mesh.vertexes = vertexes;
    file.close();
    DebugLog("Loaded mesh: " + location);
    mesh.modelLocation = location;
    return mesh;
}


//
//
//
//
//
//
//
//
//--------------------Mesh Generation--------------------------

Mesh Create2DTriangle(glm::vec3 position, glm::vec3 rotation)
{
    std::vector<float> verts =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
    };
    std::vector<float> normals =
    {
        0.0f,0.0f,1.0f,
        0.0f,0.0f,1.0f,
        0.0f,1.0f,0.0f
    };
    std::vector<float> uv =
    {
        0.0f,1.0f,
        0.0f,1.0f,
        0.0f,1.0f
    };
    std::vector<Vertex> vertxes;
    Vertex tmpvertex = Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f));
    vertxes.push_back(tmpvertex);
    tmpvertex.position = glm::vec3(1.0f, -1.0f, 0.0f);
    vertxes.push_back(tmpvertex);
    tmpvertex.position = glm::vec3(-1.0f, 1.0f, 0.0f);
    vertxes.push_back(tmpvertex);
    
    std::vector<unsigned int> indices =
    {
        0, 1, 2
    };
    return Mesh(std::to_string(TriangleMesh), vertxes, indices, position, rotation, 1);
}

Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation)
{
    std::vector<Vertex> vertexes = {
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3( 1.0f,-1.0f, 1.0f),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(1.0f,1.0f, 1.0f),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(1.0f,-1.0f, -1.0f),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(1.0f,1.0f, -1.0f),glm::vec3(0), glm::vec2(0))
    };
    std::vector<unsigned int> indices =
    {
        1, 2, 0,
        3, 6, 2,
        7, 4, 6,
        5, 0, 4,
        6, 0, 2,
        3, 5, 7,
        1, 3, 2,
        3, 7, 6,
        7, 5, 4,
        5, 1, 0,
        6, 4, 0,
        3, 1, 5
    };
    Mesh mesh = Mesh(vertexes, indices, position, rotation, 1.0f);
    mesh.modelLocation = std::to_string(CubeMesh);
    return mesh;
}

Mesh CreateSphereMesh(glm::vec3 position, glm::vec3 rotation, unsigned int subdivideNum)
{

    float t = 0.52573111f;  
    float b = 0.850650808f;

    std::vector<Vertex> vertexes = {
        Vertex(glm::vec3(-t,  b,  0),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(t,  b,  0),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(-t, -b,  0),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(t, -b,  0),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(0, -t,  b),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(0,  t,  b),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(0, -t, -b),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(0,  t, -b),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(b,  0, -t),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(b,  0,  t),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(-b,  0, -t),glm::vec3(0), glm::vec2(0)),
        Vertex(glm::vec3(-b,  0,  t),glm::vec3(0), glm::vec2(0))
    };

    std::vector<unsigned int> indices = {
        0, 11, 5, 
        0, 5, 1,
        0, 1, 7,
        0, 7, 10,
        0, 10, 11,
        
        1, 5, 9,
        5, 11, 4,
        11, 10, 2,
        10, 7, 6,
        7, 1, 8,
        
        3, 9, 4,
        3, 4, 2,
        3, 2, 6,
        3, 6, 8,
        3, 8, 9,
        
        4, 9, 5,
        2, 4, 11,
        6, 2, 10,
        8, 6, 7,
        9, 8, 1
    };

    std::vector<float> normals;
    std::vector<float> uv;

    for (unsigned int i = 0; i < subdivideNum; i++)
    {
        std::vector<unsigned int> newIndices;
        std::vector<Vertex> newVerts;
        for (unsigned int i = 0; i < indices.size(); i += 3)
        {
            //Get the required vertexes
            unsigned int ia = indices[i]; 
            unsigned int ib = indices[i + 1];
            unsigned int ic = indices[i + 2]; 
            Vertex a = vertexes[ia];
            Vertex b = vertexes[ib];
            Vertex c = vertexes[ic];

            //Create New Points
            glm::vec3 ab = glm::normalize((a.position + b.position) * 0.5f);
            glm::vec3 bc = glm::normalize((b.position + c.position) * 0.5f);
            glm::vec3 ca = glm::normalize((c.position + a.position) * 0.5f);

            //Create Normals
            glm::vec3 u = bc - ab;
            glm::vec3 v = ca - ab;
            glm::vec3 normal = glm::normalize(glm::cross(u,v));

            //Add the new vertexes
            ia = newVerts.size();
            newVerts.push_back(a);
            ib = newVerts.size();
            newVerts.push_back(b);
            ic = newVerts.size();
            newVerts.push_back(c);
            unsigned int iab = newVerts.size();
            newVerts.push_back(Vertex(ab, normal, glm::vec2(0)));
            unsigned int ibc = newVerts.size(); 
            newVerts.push_back(Vertex(bc, normal, glm::vec2(0)));
            unsigned int ica = newVerts.size(); 
            newVerts.push_back(Vertex(ca, normal, glm::vec2(0)));
            newIndices.push_back(ia); newIndices.push_back(iab); newIndices.push_back(ica);
            newIndices.push_back(ib); newIndices.push_back(ibc); newIndices.push_back(iab);
            newIndices.push_back(ic); newIndices.push_back(ica); newIndices.push_back(ibc);
            newIndices.push_back(iab); newIndices.push_back(ibc); newIndices.push_back(ica);
        }
        indices.clear();
        indices = newIndices;
        vertexes.clear();
        vertexes = newVerts;
    }
    Mesh mesh = Mesh(std::to_string(IcoSphereMesh), vertexes, indices, position, rotation, 1.0f);
    return mesh;
}

