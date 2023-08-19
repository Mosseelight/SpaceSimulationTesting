#include "../include/Core/Mesh.hpp"
#include "../include/Core/Debug.hpp"

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
    this->vao = other.vao;
    this->vbo = other.vbo;
    this->ebo = other.ebo;
    this->vertexes = other.vertexes;
    this->vertices = other.vertices;
    this->normals = other.normals;
    this->uv = other.uv;
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

Mesh::Mesh(std::vector<float> vertices, std::vector<float> normals, std::vector<float> uv, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale)
{
    this->vertices = vertices;
    this->normals = normals;
    this->uv = uv;
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

std::vector<Vertex> Mesh::CombineToVertex()
{
    std::vector<Vertex> tempVertex;
    DebugLog("no vertexes combining to vertex");
    for (unsigned int i = 0; i < vertices.size() / 3; i++)
    {
        Vertex tmpVertex = Vertex(glm::vec3(0), glm::vec3(0), glm::vec2(0));
        if(vertices.size() != 0)
            tmpVertex.position = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
        if(normals.size() != 0)
            tmpVertex.normal = glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
        if(uv.size() != 0)
            tmpVertex.uv = glm::vec2(uv[i * 2], uv[i * 2 + 1]);
        tempVertex.push_back(tmpVertex);
    }
    return tempVertex;
}

void Mesh::BufferGens()
{
    BufferLock = true;
    if(vertexes.size() == 0)
        vertexes = CombineToVertex();
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
    if(vertexes.size() == 0)
        vertexes = CombineToVertex();
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
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f,0.0f,0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f,0.0f,1.0f));
    return model;
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

    std::vector<glm::vec3> normals;
    for (unsigned int v = 0; v < vertexes.size(); v++)
    {
        glm::vec3 normal;
        for (unsigned int i = 0; i < indices.size() / 3; i++)
        {
            unsigned int a,b,c;
            a = indices[i * 3];
            b = indices[i * 3 + 1];
            c = indices[i * 3 + 2];
            if(vertexes[v].position == vertexes[a].position || vertexes[v].position == vertexes[b].position || vertexes[v].position == vertexes[c].position)
            {
                glm::vec3 u = vertexes[b].position - vertexes[a].position;
                glm::vec3 v = vertexes[c].position - vertexes[a].position;
                glm::vec3 tmpnormal = glm::cross(u,v);
                normal = tmpnormal;
            }
        }
        normals.push_back(glm::normalize(normal));
    }
    for (unsigned int v = 0; v < vertexes.size(); v++)
    {
        vertexes[v].normal = normals[v];
    }
    ReGenBuffer();
}

void Mesh::SubdivideTriangle()
{
    std::vector<unsigned int> newIndices;
    std::vector<Vertex> newVerts;
    for (unsigned int i = 0; i < indices.size() / 3; i++)
    {
        //ia is the row in the vertices array
        unsigned int ia = indices[i * 3]; 
        unsigned int ib = indices[i * 3 + 1];
        unsigned int ic = indices[i * 3 + 2]; 
        Vertex a = vertexes[ia];
        Vertex b = vertexes[ib];
        Vertex c = vertexes[ic];
        glm::vec3 ab = (a.position + b.position) * 0.5f;
        glm::vec3 bc = (b.position + c.position) * 0.5f;
        glm::vec3 ca = (c.position + a.position) * 0.5f;
        glm::vec3 u = bc - ab;
        glm::vec3 v = ca - ab;
        glm::vec3 normal = glm::cross(u,v);
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
    return Mesh(vertxes, indices, position, rotation, 1);
}

Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation)
{
    std::vector<float> verts =
    {
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f,1.0f, 1.0f,
        1.0f,-1.0f, -1.0f,
        1.0f,1.0f, -1.0f
    };
    std::vector<float> normals;
    std::vector<float> uv;
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
    return Mesh(verts, normals, uv, indices, position, rotation, 1.0f);
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
        for (unsigned int i = 0; i < indices.size() / 3; i++)
        {
            //Get the required vertexes
            unsigned int ia = indices[i * 3]; 
            unsigned int ib = indices[i * 3 + 1];
            unsigned int ic = indices[i * 3 + 2]; 
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
            glm::vec3 normal = glm::cross(u,v);

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

    return Mesh(vertexes, indices, position, rotation, 1.0f);
}

