#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"

struct Simplex
{
    glm::vec3 a, a2, a3, b, b2, b3, c, c2, c3, d, d2, d3;
    int count;

    Simplex() 
    {
        a, a2, a3, b, b2, b3, c, c2, c3, d, d2, d3 = glm::vec3(0.0f);
    }
};

struct SpatialObject;

struct SimplexSolve
{
    glm::vec3 pos;
    bool check;

    SimplexSolve() {}
    SimplexSolve(glm::vec3 pos, bool check) : pos(pos), check(check) {}
};

bool CollisionCheck(SpatialObject own, SpatialObject other);