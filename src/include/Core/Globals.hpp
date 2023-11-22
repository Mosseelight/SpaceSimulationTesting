#pragma once
#include <string>

const int SCRWIDTH = 1920;
const int SCRHEIGHT = 1080;
const std::string EngVer = "0.6.8";
const unsigned int ShadowSize = 16384;

//Physics
const int maxPhysicSteps = 3; // if fps is lower than 20 then the simulation will start skipping calculations for the missed time
const float PhysicsStep = 0.016f;
extern float PhysicsSpeed;
const float AirDensity = 0.1f;

extern int DrawCallCount;
extern float drawCallAvg;