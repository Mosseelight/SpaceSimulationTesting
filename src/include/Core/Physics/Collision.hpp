#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"
#include "../Math.hpp"
#include "libccd/ccd/ccd.h"

class CollisionPoint
{
public:
    glm::vec3 point;
    glm::vec3 normal;
    float dist;

    CollisionPoint();
    CollisionPoint(glm::vec3 normal, float dist);
    CollisionPoint(glm::vec3 point, glm::vec3 normal, float dist);
    ~CollisionPoint();
};

std::pair<bool, CollisionPoint> CollisionCheckNarrowGjk(SpatialObject& own, SpatialObject& other);
std::pair<bool, CollisionPoint> CollisionCheckNarrowSat(SpatialObject& obj1, SpatialObject& obj2);
bool CollisionCheckBroad(SpatialObject& own, SpatialObject& other);