#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"

std::pair<bool, glm::vec3> CollisionCheckNarrow(SpatialObject& own, SpatialObject& other);
bool CollisionCheckBroad(SpatialObject& own, SpatialObject& other);