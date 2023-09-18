#include "../../include/Core/Physics/Chunks.hpp"

Chunk::Chunk()
{

}

Chunk::~Chunk()
{

}

void Chunk::FindSpaceObjects(std::vector<SpatialObject>& objects)
{
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        BoundingBox boxOther = objects[i].SO_rigidbody.boundbox;
        if (min.x <= boxOther.max.x && max.x >= boxOther.min.x && min.y <= boxOther.max.y && max.y >= boxOther.min.y
        && min.z <= boxOther.max.z && max.z >= boxOther.min.z) 
        {
            ids.push_back(objects[i].SO_id);
        }
    }
    
}




ChunkManager::ChunkManager()
{

}

void ChunkManager::InitChunks(std::vector<SpatialObject>& objects)
{
    minChunkSize = glm::vec3(0);
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        minChunkSize += objects[i].SO_rigidbody.boundbox.min;
    }
    float size = std::min(std::min(minChunkSize.x / objects.size(), minChunkSize.y / objects.size()), minChunkSize.z / objects.size());
    minChunkSize = glm::vec3(size);
    maxChunkSize = -minChunkSize;
}

void ChunkManager::UpdateChunks(std::vector<SpatialObject>& objects)
{
    
}

void ChunkManager::CreateChunks(std::vector<SpatialObject>& objects)
{
    Chunk chunk;
    //get chunk pos
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        chunk.move.x = floor(objects[i].SO_rigidbody.position.x / abs(minChunkSize.x * 2));
        chunk.move.y = floor(objects[i].SO_rigidbody.position.y / abs(minChunkSize.y * 2));
        chunk.move.z = floor(objects[i].SO_rigidbody.position.z / abs(minChunkSize.z * 2));
    }
    //dont add chunk if there
    for (unsigned int i = 0; i < chunks.size(); i++)
    {
        if(chunk.move == chunks[i].move)
        {
            return;
        }
    }
    
    
}