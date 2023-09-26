#include "../../include/Core/Physics/Chunks.hpp"

Chunk::Chunk()
{
    this->min = glm::vec3(-10.0f);
    this->max = glm::vec3(10.0f);
    this->depth = 0;
}

Chunk::Chunk(glm::vec3 max, glm::vec3 min, unsigned int depth)
{
    this->max = max;
    this->min = min;
    this->depth = depth;
}

Chunk::~Chunk()
{
    
}

void Chunk::CreateSubChunks()
{
    if(chunks[0] == NULL)
        chunks[0].reset(new Chunk(glm::vec3(max.x, max.y, max.z), glm::vec3((max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f), depth + 1));
    if(chunks[1] == NULL)
        chunks[1].reset(new Chunk(glm::vec3(max.x, (max.y + min.y) * 0.5f, max.z), glm::vec3((max.x + min.x) * 0.5f, min.y, (max.z + min.z) * 0.5f), depth + 1));
    if(chunks[2] == NULL)
        chunks[2].reset(new Chunk(glm::vec3((max.x + min.x) * 0.5f, max.y, max.z), glm::vec3(min.x, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f), depth + 1));
    if(chunks[3] == NULL)
        chunks[3].reset(new Chunk(glm::vec3((max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, max.z), glm::vec3(min.x, min.y, (max.z + min.z) * 0.5f), depth + 1));
    if(chunks[4] == NULL)
        chunks[4].reset(new Chunk(glm::vec3(max.x, max.y, (max.z + min.z) * 0.5f), glm::vec3((max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, min.z), depth + 1));
    if(chunks[5] == NULL)
        chunks[5].reset(new Chunk(glm::vec3(max.x, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f), glm::vec3((max.x + min.x) * 0.5f, min.y, min.z), depth + 1));
    if(chunks[6] == NULL)
        chunks[6].reset(new Chunk(glm::vec3((max.x + min.x) * 0.5f, max.y, (max.z + min.z) * 0.5f), glm::vec3(min.x, (max.y + min.y) * 0.5f, min.z), depth + 1));
    if(chunks[7] == NULL)
        chunks[7].reset(new Chunk(glm::vec3((max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f), glm::vec3(min.x, min.y, min.z), depth + 1));
}

bool Chunk::InsertChunk(SpatialObject& object)
{
    std::cout << (!inChunk(object.SO_rigidbody.boundbox)) << " " << object.SO_id << std::endl;
    if(!inChunk(object.SO_rigidbody.boundbox))
        return false;
    
    if(objects.size() < 1)
    {
        objects.push_back(std::make_unique<SpatialObject>(object));
        return true;
    }

    CreateSubChunks();

    for (int i = 0; i < 8; ++i)
    {
        if (chunks[i]->InsertChunk(object))
        {
            return true;
        }
    }

    return false;
    
}

void Chunk::DrawChunks()
{
    DrawDebugCube(glm::vec3(max), 0.1f, glm::vec3(1, 0, 0));
    DrawDebugCube(glm::vec3(min), 0.1f, glm::vec3(0, 255, 0));
    DrawDebugLine(glm::vec3(max), glm::vec3(min), glm::vec3(0, 0, 255));

    for (int i = 0; i < 8; ++i)
    {
        if (chunks[i] != NULL)
        {
            chunks[i]->DrawChunks();
        }
    }
    
}

/*std::vector<SpatialObject&> Chunk::SearchChunk(Chunk& chunk)
{
    return;
}*/

bool Chunk::inChunk(BoundingBox box)
{
    return (box.min.x <= max.x && box.max.x >= min.x && box.min.y <= max.y && box.max.y >= min.y
    && box.min.z <= max.z && box.max.z >= min.z);
}