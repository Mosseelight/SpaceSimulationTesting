#pragma once
#include "../Globals.hpp"
#include "../Scene.hpp"
#include "Chunks.hpp"

class Scene;

void RunSimulation(float deltaTime, Scene& scene, int runSim);
void UpdateChunks(std::vector<SpatialObject>& SpatialObjects);