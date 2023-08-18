#pragma once
#include <iostream>
#include <string>
#include <fstream> 
#include <cstring>
#include <sstream>
#include "Mesh.hpp"
#include "Debug.hpp"

std::string ReadFile(std::string location);
bool FileExist(std::string location);
void CreateFile(std::string location, std::string name);
void WriteFile(std::string location, std::string content);
Mesh LoadModel(glm::vec3 position, glm::vec3 rotation, std::string location);
void LoadTexture();
float GetRamUsage();