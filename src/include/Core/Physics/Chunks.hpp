#pragma once
#include "../Scene.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

class Chunk
{
public:
    glm::vec3 min, max;
    glm::vec3 move;
    std::vector<unsigned int> ids;
    void FindSpaceObjects(std::vector<SpatialObject>& objects);
    Chunk();
    ~Chunk();
};

class ChunkManager
{
public:
    std::vector<Chunk> chunks;
    //std::vector<unsigned int> ids;
    glm::vec3 minChunkSize;
    glm::vec3 maxChunkSize;
    ChunkManager();
    void InitChunks(std::vector<SpatialObject>& objects);
    void UpdateChunks(std::vector<SpatialObject>& objects);
    void CreateChunks(std::vector<SpatialObject>& objects);
};
