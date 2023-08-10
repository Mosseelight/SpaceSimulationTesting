#pragma once
#include <iostream>
#include <string>
#include <fstream> 
#include <cstring>
#include <sstream>
#include "Mesh.hpp"

std::string ReadFile(std::string location);
void LoadModel(Mesh *mesh, std::string location);
void LoadTexture();
float GetRamUsage();