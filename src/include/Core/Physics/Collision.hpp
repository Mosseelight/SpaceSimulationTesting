#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"

struct Simplex
{
    glm::vec3 a, b, c, d;
    int count;

    Simplex() 
    {
        a, b, c, d = glm::vec3(0.0f);
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