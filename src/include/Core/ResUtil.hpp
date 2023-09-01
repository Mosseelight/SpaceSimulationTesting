#pragma once
#include "Debug.hpp"
#include <iostream>
#include <string>
#include <fstream> 
#include <cstring>
#include <sstream>

std::string ReadFile(std::string location);
bool FileExist(std::string location);
void CreateFile(std::string location, std::string name);
void WriteFile(std::string location, std::string content);
std::string stringRemove(std::string s, std::string del, std::string adder);
float GetRamUsage();