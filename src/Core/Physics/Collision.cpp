#include "../../include/Core/Physics/Collision.hpp"

glm::vec3 Simplex2(glm::vec3 dir);
glm::vec3 Simplex3(glm::vec3 dir);
glm::vec3 Simplex2Return(glm::vec3 mid, glm::vec3 inva);
SimplexSolve Simplex4(glm::vec3 dir);
glm::vec3 GetVertex(std::vector<Vertex> vertexes, glm::vec3 dir, glm::mat4 model);
bool SameLine(glm::vec3 a, glm::vec3 b);

Simplex a;
bool CollisionCheck(SpatialObject own, SpatialObject other)
{
    Simplex a;
    glm::vec3 v1 = own.SO_mesh.vertexes[0].position;
    glm::vec3 v2 = other.SO_mesh.vertexes[0].position;
    glm::vec3 direction;

    a.count = 1;
    direction = v1 - v2;
    //finds the closest points
    a.a = GetVertex(other.SO_mesh.vertexes, direction, other.SO_mesh.GetModelMat()) - GetVertex(own.SO_mesh.vertexes, -direction, own.SO_mesh.GetModelMat());
    direction = -a.a;
    
    for (unsigned int i = 0; i < 64; i++)
    {
        a.d = a.c;
        a.c = a.b;
        a.b = a.a;
        a.count++;

        a.a = GetVertex(other.SO_mesh.vertexes, direction, other.SO_mesh.GetModelMat()) - GetVertex(own.SO_mesh.vertexes, -direction, own.SO_mesh.GetModelMat());

        if(glm::dot(a.a, direction) < 0)
            break;

        if(a.count == 2)
        {
            direction = Simplex2(direction);
        }
        if(a.count == 3)
        {
            direction = Simplex3(direction);
        }
        if(a.count == 4)
        {
            SimplexSolve s;
            s = Simplex4(direction);
            if(s.check)
            {
                return true;
            }
        }
    }
    return false;
    
}

glm::vec3 Simplex2(glm::vec3 dir) //line
{
    glm::vec3 mid = a.b - a.a;
    glm::vec3 inva = -a.a;

    if(SameLine(mid, inva))
    {
        return glm::cross(glm::cross(mid, inva), mid);
    }
    else
    {
        a.count = 1;
        return inva;
    }
}

glm::vec3 Simplex3(glm::vec3 dir) //triangle
{
    glm::vec3 tri = glm::cross(a.b - a.a, a.c - a.a);
    glm::vec3 mid = a.c - a.a;
    glm::vec3 inva = -a.a;

    if(SameLine(glm::cross(tri, mid), inva))
    {
        if(SameLine(mid, inva))
        {
            a.b = a.c;
            a.count = 2;
            return glm::cross(glm::cross(mid, inva), mid);
        }
        else
        {
            glm::vec3 mid2 = a.b - a.a;
            return Simplex2Return(mid2, inva);
        }
    }
    else
    {
        glm::vec3 mid2 = a.b = a.a;
        if(SameLine(glm::cross(mid2, tri), inva))
        {
            return Simplex2Return(mid2, inva);
        }
        else
        {
            if(SameLine(tri, inva))
            {
                return tri;
            }
            else
            {
                glm::vec3 temp1, temp2;
                temp1 = a.b;
                temp2 = a.c;
                a.c = temp1;
                a.b = temp2;
                return -tri;
            }
        }
    }
}

glm::vec3 Simplex2Return(glm::vec3 mid, glm::vec3 inva)
{
    if(SameLine(mid, inva))
    {
        a.count = 2;
        return glm::cross(glm::cross(mid, inva), mid);
    }
    else
    {
        a.count = 1;
        return inva;
    }
}

SimplexSolve Simplex4(glm::vec3 dir) //tet
{
    glm::vec3 tri1 = glm::cross(a.b - a.a, a.c - a.a);
    glm::vec3 tri2 = glm::cross(a.c - a.a, a.d - a.a);
    glm::vec3 tri3 = glm::cross(a.d - a.a, a.b - a.a);
    glm::vec3 inva = -a.a;

    if(SameLine(tri1, inva))
    {
        a.count = 3;
        glm::vec3 solve = Simplex3(dir);
        return SimplexSolve(solve, false);
    }

    if(SameLine(tri2, inva))
    {
        a.b = a.c;
        a.c = a.d;
        a.count = 3;
        glm::vec3 solve = Simplex3(dir);
        return SimplexSolve(solve, false);
    }

    if(SameLine(tri3, inva))
    {
        a.c = a.b;
        a.b = a.d;
        a.count = 3;
        glm::vec3 solve = Simplex3(dir);
        return SimplexSolve(solve, false);
    }

    return SimplexSolve(glm::vec3(0.0f), true);
}

glm::vec3 GetVertex(std::vector<Vertex> vertexes, glm::vec3 dir, glm::mat4 model)
{
    glm::vec3 dirModel = glm::vec3(glm::inverse(model) * glm::vec4(dir, 0.0f));
    float largestDot = glm::dot(vertexes[0].position, dirModel);
    glm::vec3 largestPos = vertexes[0].position;

    for (unsigned int i = 0; i < vertexes.size(); i++)
    {
        glm::vec3 vertexPos = vertexes[i].position;
        float currentDot = glm::dot(vertexPos, dirModel);

        if(currentDot > largestDot)
        {
            largestDot = currentDot;
            largestPos = vertexPos;
        }
    }
    largestPos = glm::vec3(model * glm::vec4(largestPos, 1.0f));
    return largestPos;
}

bool SameLine(glm::vec3 a, glm::vec3 b)
{
    return glm::dot(a,b) > 0;
}