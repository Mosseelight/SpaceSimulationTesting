#include "../include/Core/Mesh.hpp"

Mesh::Mesh(std::vector<float> vertexes, std::vector<unsigned int> indices, glm::vec3 position, glm::vec3 rotation, float scale)
{
    this->vertexes = vertexes;
    this->indices = indices;
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

Mesh::~Mesh()
{
    
}

void Mesh::BufferGens()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(float), vertexes.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*) 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::DrawMesh()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

glm::mat4 Mesh::GetModelMat()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f,0.0f,0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f,0.0f,1.0f));
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    return model;
}

void Mesh::SubdivideTriangle()
{
    std::vector<unsigned int> newIndices;
    for (unsigned int i = 0; i < indices.size() / 3; i++)
    {
        //ia is the row in the vertices array
        unsigned int ia = indices[i * 3]; 
        unsigned int ib = indices[i * 3 + 1];
        unsigned int ic = indices[i * 3 + 2]; 
        glm::vec3 a = glm::vec3(vertexes[ia * 3], vertexes[ia * 3 + 1], vertexes[ia * 3 + 2]);
        glm::vec3 b = glm::vec3(vertexes[ib * 3], vertexes[ib * 3 + 1], vertexes[ib * 3 + 2]);
        glm::vec3 c = glm::vec3(vertexes[ic * 3], vertexes[ic * 3 + 1], vertexes[ic * 3 + 2]);
        glm::vec3 ab = (a + b) * 0.5f;
        glm::vec3 bc = (b + c) * 0.5f;
        glm::vec3 ca = (c + a) * 0.5f;
        unsigned int iab = vertexes.size() / 3; 
        vertexes.push_back(ab.x);
        vertexes.push_back(ab.y);
        vertexes.push_back(ab.z);
        unsigned int ibc = vertexes.size() / 3; 
        vertexes.push_back(bc.x);
        vertexes.push_back(bc.y);
        vertexes.push_back(bc.z);
        unsigned int ica = vertexes.size() / 3; 
        vertexes.push_back(ca.x); 
        vertexes.push_back(ca.y); 
        vertexes.push_back(ca.z);
        newIndices.push_back(ia); newIndices.push_back(iab); newIndices.push_back(ica);
        newIndices.push_back(ib); newIndices.push_back(ibc); newIndices.push_back(iab);
        newIndices.push_back(ic); newIndices.push_back(ica); newIndices.push_back(ibc);
        newIndices.push_back(iab); newIndices.push_back(ibc); newIndices.push_back(ica);
    }
    for (unsigned int i = 0; i < newIndices.size(); i++)
    {
        indices.push_back(newIndices[i]);
    }
    
}


//--------------------Mesh Generation--------------------------

Mesh Create2DTriangle(glm::vec3 position, glm::vec3 rotation)
{
    std::vector<float> vertices =
    {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 2
    };

    return Mesh(vertices, indices, position, rotation, 1);
}

Mesh CreateCubeMesh(glm::vec3 position, glm::vec3 rotation)
{
    std::vector<float> vertices =
    {
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
        3, 2, 6,
        6, 7, 3
    };
    return Mesh(vertices, indices, position, rotation, 1.0f);
}

Mesh CreateSphereMesh(glm::vec3 position, glm::vec3 rotation, unsigned int subdivideNum)
{

    float t = 0.52573111f;
    float b = 0.850650808f;

    std::vector<float> vertices = {
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

    for (unsigned int i = 0; i < subdivideNum; i++)
    {
        std::vector<unsigned int> newIndices;
        for (unsigned int i = 0; i < indices.size() / 3; i++)
        {
            //ia is the row in the vertices array
            unsigned int ia = indices[i * 3]; 
            unsigned int ib = indices[i * 3 + 1];
            unsigned int ic = indices[i * 3 + 2]; 
            glm::vec3 a = glm::vec3(vertices[ia * 3], vertices[ia * 3 + 1], vertices[ia * 3 + 2]);
            glm::vec3 b = glm::vec3(vertices[ib * 3], vertices[ib * 3 + 1], vertices[ib * 3 + 2]);
            glm::vec3 c = glm::vec3(vertices[ic * 3], vertices[ic * 3 + 1], vertices[ic * 3 + 2]);
            glm::vec3 ab = glm::normalize((a + b) * 0.5f);
            glm::vec3 bc = glm::normalize((b + c) * 0.5f);
            glm::vec3 ca = glm::normalize((c + a) * 0.5f);
            unsigned int iab = vertices.size() / 3; 
            vertices.push_back(ab.x);
            vertices.push_back(ab.y);
            vertices.push_back(ab.z);
            unsigned int ibc = vertices.size() / 3; 
            vertices.push_back(bc.x);
            vertices.push_back(bc.y);
            vertices.push_back(bc.z);
            unsigned int ica = vertices.size() / 3; 
            vertices.push_back(ca.x); 
            vertices.push_back(ca.y); 
            vertices.push_back(ca.z);
            newIndices.push_back(ia); newIndices.push_back(iab); newIndices.push_back(ica);
            newIndices.push_back(ib); newIndices.push_back(ibc); newIndices.push_back(iab);
            newIndices.push_back(ic); newIndices.push_back(ica); newIndices.push_back(ibc);
            newIndices.push_back(iab); newIndices.push_back(ibc); newIndices.push_back(ica);
        }
        for (unsigned int i = 0; i < newIndices.size(); i++)
        {
            indices.push_back(newIndices[i]);
        }
        
    }

    return Mesh(vertices, indices, position, rotation, 1.0f);
}

