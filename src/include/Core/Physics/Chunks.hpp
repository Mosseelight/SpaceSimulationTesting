#pragma once
#include "../Scene.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

class Chunk
{
public:
    glm::vec3 min;
    glm::vec3 max;

    SpatialObject *chunkObject;

    Chunk *TLChunk;
    Chunk *TRChunk;
    Chunk *BLChunk;
    Chunk *BRChunk;
    Chunk *TLBChunk;
    Chunk *TRBChunk;
    Chunk *BLBChunk;
    Chunk *BRBChunk;

    Chunk();
    Chunk(glm::vec3 min, glm::vec3 max);
    ~Chunk();

    void InsertChunk(SpatialObject& object);
    SpatialObject* SearchChunk(glm::vec3 pos);
    bool inChunk(glm::vec3 pos);
};
