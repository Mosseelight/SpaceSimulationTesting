#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"

bool CollisionCheckNarrow(SpatialObject& own, SpatialObject& other);
bool CollisionCheckBroad(SpatialObject& own, SpatialObject& other);