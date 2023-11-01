#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"
#include "../Math.hpp"

class CollisionPoint
{
public:
    glm::vec3 point;
    glm::vec3 normal;
    float dist;

    CollisionPoint();
    CollisionPoint(glm::vec3 normal, float dist);
    ~CollisionPoint();
};

std::pair<bool, CollisionPoint> CollisionCheckNarrow(SpatialObject& own, SpatialObject& other, int type);
bool CollisionCheckBroad(SpatialObject& own, SpatialObject& other);
bool CollisionCheckBroader(SpatialObject& own, SpatialObject& other);