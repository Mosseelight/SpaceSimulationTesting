#pragma once
#include "../Scene.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <array>
#include <memory>

class Chunk
{
public:
    unsigned int depth;
    glm::vec3 min;
    glm::vec3 max;

    size_t objectCount;
    std::vector<std::shared_ptr<SpatialObject>> objects;
    std::array<std::unique_ptr<Chunk>, 8> chunks;

    Chunk();
    Chunk(glm::vec3 min, glm::vec3 max, unsigned int depth);
    ~Chunk();

    bool InsertChunk(SpatialObject& object);
    void CreateSubChunks();
    void DrawChunks();
    //std::vector<SpatialObject&> SearchChunk(Chunk& chunk);
    bool inChunk(glm::vec3 position);
};
