#include "../../include/Core/Physics/Chunks.hpp"

std::vector<Chunk*> totalChunks;
Chunk::Chunk()
{
    min = glm::vec3(-10.0f);
    max = glm::vec3(10.0f);
    depth = 0;
    objectCount = 0;
}

Chunk::Chunk(glm::vec3 min, glm::vec3 max, unsigned int depth)
{
    this->min = min;
    this->max = max;
    this->depth = depth;
    objectCount = 0;
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
    if(!inChunk(object.SO_rigidbody.position))
        return false;
    
    bool root = chunks[0] == NULL && chunks[1] == NULL && chunks[2] == NULL && chunks[3] == NULL && chunks[4] == NULL && chunks[5] == NULL && chunks[6] == NULL && chunks[7] == NULL;
    if(objectCount < 5)
    {
        objectCount++;
        objects.push_back(std::make_shared<SpatialObject>(object));
        return true;
    }

    if(root)
        CreateSubChunks();
    
    for (size_t i = 0; i < 8; i++)
    {
        std::cout << chunks[i]->depth << std::endl;
        totalChunks.push_back(chunks[i].get());
    }
    

    if(chunks[0]->InsertChunk(object) && depth < 4)
        return true;
    if(chunks[1]->InsertChunk(object) && depth < 4)
        return true;
    if(chunks[2]->InsertChunk(object) && depth < 4)
        return true;
    if(chunks[3]->InsertChunk(object) && depth < 4)
        return true;
    if(chunks[4]->InsertChunk(object) && depth < 4)
        return true;
    if(chunks[5]->InsertChunk(object) && depth < 4)
        return true;
    if(chunks[6]->InsertChunk(object) && depth < 4)
        return true;
    if(chunks[7]->InsertChunk(object) && depth < 4)
        return true;

    objectCount++;
    objects.push_back(std::make_shared<SpatialObject>(object));
    return false;
    
}

void Chunk::DrawChunks()
{
    for (size_t i = 0; i < totalChunks.size(); i++)
    {
        DrawDebugCube(totalChunks[i]->max, 0.4f, glm::vec3(1, 0, 0));
        DrawDebugCube(totalChunks[i]->min, 0.4f, glm::vec3(0, 255, 0));
        DrawDebugLine(totalChunks[i]->max, totalChunks[i]->min, glm::vec3(0, 0, 255));
    }
    
}

/*std::vector<SpatialObject&> Chunk::SearchChunk(Chunk& chunk)
{
    return;
}*/

bool Chunk::inChunk(glm::vec3 pos)
{
    return (pos.x <= max.x && pos.x >= min.x && pos.y <= max.y && pos.y >= min.y && pos.z <= max.z && pos.z >= min.z);
}