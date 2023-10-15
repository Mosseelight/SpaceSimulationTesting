#pragma once
#include "../Scene.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

class ChunkManager
{
public:
    float ChunkSize;
    std::vector<std::pair<unsigned int, unsigned int>> spatialLookup;
    std::vector<unsigned int> startLookup;

    ChunkManager();
    ~ChunkManager();
    std::vector<unsigned int> FindObjectsInChunk(std::vector<SpatialObject>& objects, unsigned int objectID);
    void UpdateChunks(std::vector<SpatialObject>& objects);
    glm::vec3 getChunkpos(glm::vec3 pos);
    unsigned int getHashVal(glm::vec3 pos);
    unsigned int getKeyVal(unsigned int hash);
};
