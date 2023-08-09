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
    std::cout << "no vertexes combining to vertex" << std::endl;
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
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*) 0);

    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 3 * sizeof(float), (void*) 0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f,0.0f,0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f,0.0f,1.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    return model;
}

void Mesh::SubdivideTriangle()
{
    std::vector<unsigned int> newIndices;
    std::vector<float> newVerts;
    for (unsigned int i = 0; i < indices.size() / 3; i++)
    {
        //ia is the row in the vertices array
        unsigned int ia = indices[i * 3]; 
        unsigned int ib = indices[i * 3 + 1];
        unsigned int ic = indices[i * 3 + 2]; 
        glm::vec3 a = glm::vec3(vertices[ia * 3], vertices[ia * 3 + 1], vertices[ia * 3 + 2]);
        glm::vec3 b = glm::vec3(vertices[ib * 3], vertices[ib * 3 + 1], vertices[ib * 3 + 2]);
        glm::vec3 c = glm::vec3(vertices[ic * 3], vertices[ic * 3 + 1], vertices[ic * 3 + 2]);
        glm::vec3 ab = (a + b) * 0.5f;
        glm::vec3 bc = (b + c) * 0.5f;
        glm::vec3 ca = (c + a) * 0.5f;
        ia = newVerts.size() / 3;
        newVerts.push_back(a.x);
        newVerts.push_back(a.y);
        newVerts.push_back(a.z);
        ib = newVerts.size() / 3;
        newVerts.push_back(b.x);
        newVerts.push_back(b.y);
        newVerts.push_back(b.z);
        ic = newVerts.size() / 3;
        newVerts.push_back(c.x);
        newVerts.push_back(c.y);
        newVerts.push_back(c.z);
        unsigned int iab = newVerts.size() / 3;
        newVerts.push_back(ab.x);
        newVerts.push_back(ab.y);
        newVerts.push_back(ab.z);
        unsigned int ibc = newVerts.size() / 3; 
        newVerts.push_back(bc.x);
        newVerts.push_back(bc.y);
        newVerts.push_back(bc.z);
        unsigned int ica = newVerts.size() / 3; 
        newVerts.push_back(ca.x); 
        newVerts.push_back(ca.y); 
        newVerts.push_back(ca.z);
        newIndices.push_back(ia); newIndices.push_back(iab); newIndices.push_back(ica);
        newIndices.push_back(ib); newIndices.push_back(ibc); newIndices.push_back(iab);
        newIndices.push_back(ic); newIndices.push_back(ica); newIndices.push_back(ibc);
        newIndices.push_back(iab); newIndices.push_back(ibc); newIndices.push_back(ica);
    }
    indices.clear();
    indices = newIndices;
    vertices.clear();
    vertices = newVerts;
}


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

    std::vector<float> verts = {
        -t,  b,  0,
        t,  b,  0,
        -t, -b,  0,
        t, -b,  0,
        0, -t,  b,
        0,  t,  b,
        0, -t, -b,
        0,  t, -b,
        b,  0, -t,
        b,  0,  t,
        -b,  0, -t,
        -b,  0,  t
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
        std::vector<float> newVerts;
        for (unsigned int i = 0; i < indices.size() / 3; i++)
        {
            //ia is the row in the vertices array
            unsigned int ia = indices[i * 3]; 
            unsigned int ib = indices[i * 3 + 1];
            unsigned int ic = indices[i * 3 + 2]; 
            glm::vec3 a = glm::vec3(verts[ia * 3], verts[ia * 3 + 1], verts[ia * 3 + 2]);
            glm::vec3 b = glm::vec3(verts[ib * 3], verts[ib * 3 + 1], verts[ib * 3 + 2]);
            glm::vec3 c = glm::vec3(verts[ic * 3], verts[ic * 3 + 1], verts[ic * 3 + 2]);
            glm::vec3 ab = glm::normalize((a + b) * 0.5f);
            glm::vec3 bc = glm::normalize((b + c) * 0.5f);
            glm::vec3 ca = glm::normalize((c + a) * 0.5f);
            ia = newVerts.size() / 3;
            newVerts.push_back(a.x);
            newVerts.push_back(a.y);
            newVerts.push_back(a.z);
            ib = newVerts.size() / 3;
            newVerts.push_back(b.x);
            newVerts.push_back(b.y);
            newVerts.push_back(b.z);
            ic = newVerts.size() / 3;
            newVerts.push_back(c.x);
            newVerts.push_back(c.y);
            newVerts.push_back(c.z);
            unsigned int iab = newVerts.size() / 3;
            newVerts.push_back(ab.x);
            newVerts.push_back(ab.y);
            newVerts.push_back(ab.z);
            unsigned int ibc = newVerts.size() / 3; 
            newVerts.push_back(bc.x);
            newVerts.push_back(bc.y);
            newVerts.push_back(bc.z);
            unsigned int ica = newVerts.size() / 3; 
            newVerts.push_back(ca.x); 
            newVerts.push_back(ca.y); 
            newVerts.push_back(ca.z);
            newIndices.push_back(ia); newIndices.push_back(iab); newIndices.push_back(ica);
            newIndices.push_back(ib); newIndices.push_back(ibc); newIndices.push_back(iab);
            newIndices.push_back(ic); newIndices.push_back(ica); newIndices.push_back(ibc);
            newIndices.push_back(iab); newIndices.push_back(ibc); newIndices.push_back(ica);
        }
        indices.clear();
        indices = newIndices;
        verts.clear();
        verts = newVerts;
    }

    return Mesh(verts, normals, uv, indices, position, rotation, 1.0f);
}

