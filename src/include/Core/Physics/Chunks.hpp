#pragma once
#include "../Scene.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

class Chunk
{   
public:
    unsigned int key;
    unsigned int id;
    bool sleep;
};

class ChunkManager
{
public:
    float ChunkSize;
    std::vector<std::tuple<int, unsigned int, bool>> spatialLookup;
    std::vector<unsigned int> startLookup;
    std::vector<unsigned int> chunkOffsets;

    ChunkManager();
    ~ChunkManager();
    std::vector<unsigned int> FindObjectsInChunk(std::vector<SpatialObject>& objects, unsigned int objectID);
    void UpdateChunks(std::vector<SpatialObject>& objects);
    glm::vec3 getChunkpos(glm::vec3 pos);
    int getHashVal(glm::vec3 pos);
    int getKeyVal(int hash);
};
