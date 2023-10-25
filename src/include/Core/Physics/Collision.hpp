#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"
#include "../Math.hpp"

class CollisionPoint
{
public:
    glm::vec3 point;
    float dist;

    CollisionPoint();
    CollisionPoint(glm::vec3 point, float dist);
    ~CollisionPoint();
};

/// @brief 0 for taking in the vertexes of the mesh and 1 for using the orientied bounding box
/// @return 
std::pair<bool, CollisionPoint> CollisionCheckNarrow(SpatialObject& own, SpatialObject& other, unsigned int type);
bool CollisionCheckBroad(SpatialObject& own, SpatialObject& other);
bool CollisionCheckBroader(SpatialObject& own, SpatialObject& other);