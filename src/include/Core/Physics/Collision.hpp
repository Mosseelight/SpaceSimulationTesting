#pragma once
#include <glm/glm.hpp>
#include "../Scene.hpp"
#include "../Debug.hpp"

struct SpatialObject;

bool CollisionCheckNarrow(SpatialObject own, SpatialObject other);