#pragma once
#include "../glad/glad.h"
#include "glm/glm.hpp"
#include "ResUtil.hpp"
#include "Shader.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

struct DebugShaderStuff
{
    glm::mat4 m_proj;
    glm::mat4 m_view;
    std::string m_shaderLoc;
};

void DebugLog(std::string content);
void CreateFileLog(std::string location);
void DrawDebugLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
void DrawDebugItems();
void SetNeededDebug(glm::mat4 proj, glm::mat4 view, std::string shaderLoc);