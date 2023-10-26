#include "../../include/Core/Physics/Chunks.hpp"

ChunkManager::ChunkManager()
{
    ChunkSize = 10;
}

ChunkManager::~ChunkManager()
{

}

std::vector<unsigned int> ChunkManager::FindObjectsInChunk(std::vector<SpatialObject>& objects, unsigned int objectID)
{
    //use the start lookup array maybe?
    std::vector<unsigned int> objectsInSameChunk;
    glm::vec3 pos = getChunkpos(objects[objectID].SO_rigidbody.position);
    unsigned int key = getKeyVal(getHashVal(pos));
    unsigned int startIndex = startLookup[key];

    for (unsigned int i = startIndex; i < spatialLookup.size(); i++)
    {
        unsigned int currentKey = spatialLookup[i].first;
        if (currentKey == key && objects[spatialLookup[i].second].SO_id != objectID)
            objectsInSameChunk.push_back(objects[spatialLookup[i].second].SO_id);
        else
            break;
    }

    return objectsInSameChunk;
}

unsigned int objectCount = 0;
void ChunkManager::UpdateChunks(std::vector<SpatialObject>& objects)
{

    //array space allocation to fit any new objects or objects that span multiple chunks
    if(objectCount == 0 || objectCount != objects.size())
    {
        spatialLookup.resize(objects.size());
        startLookup.resize(objects.size());
        chunkOffsets.resize(objects.size() + 1);
        unsigned int spanCount = 0;
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            unsigned int offset = 0;
            if(glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / ChunkSize > 1)
            {
                spanCount++;
                glm::vec3 min = objects[i].SO_rigidbody.boundbox.min;
                glm::vec3 max = objects[i].SO_rigidbody.boundbox.max;
                
                // This is really stupid I need to find a better way

                if(min.x == 0 && max.x == 0)
                {
                    for (int y = min.y; y < max.y; y += ChunkSize)
                    {
                        for (int z = min.z; z < max.z; z += ChunkSize)
                        {
                            spatialLookup.push_back(std::make_pair(0, 0));
                            startLookup.push_back(0);
                            offset++;
                        }
                    }
                }
                else if(min.y == 0 && max.y == 0)
                {
                    for (int x = min.x; x < max.x; x += ChunkSize)
                    {
                        for (int z = min.z; z < max.z; z += ChunkSize)
                        {
                            spatialLookup.push_back(std::make_pair(0, 0));
                            startLookup.push_back(0);
                            offset++;
                        }
                    }
                }
                else if(min.z == 0 && max.z == 0)
                {
                    for (int x = min.x; x < max.x; x += ChunkSize)
                    {
                        for (int y = min.y; y < max.y; y += ChunkSize)
                        {
                            spatialLookup.push_back(std::make_pair(0, 0));
                            startLookup.push_back(0);
                            offset++;
                        }
                    }
                }
                else
                {
                    for (int x = min.x; x < max.x; x += ChunkSize)
                    {
                        for (int y = min.y; y < max.y; y += ChunkSize)
                        {
                            for (int z = min.z; z < max.z; z += ChunkSize)
                            {
                                spatialLookup.push_back(std::make_pair(0, 0));
                                startLookup.push_back(0);
                                offset++;
                            }
                        }
                    }
                }
                chunkOffsets[i + 1] = offset * spanCount;
            }
            else
                chunkOffsets[i + 1] = 0;
        }
    }

    //add the keys to the array with the corresponding id 
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        if(glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / ChunkSize < 1)
        {
            glm::vec3 pos = getChunkpos(objects[i].SO_rigidbody.position);
            unsigned int key = getKeyVal(getHashVal(pos));
            spatialLookup[i] = std::make_pair(key, i);
            startLookup[i] = UINT32_MAX;
        }
        else
        {
            glm::vec3 min = objects[i].SO_rigidbody.boundbox.min;
            glm::vec3 max = objects[i].SO_rigidbody.boundbox.max;
            unsigned int count = 0;

            // find a replacement for the if statements if there is any

            if(min.x == 0 && max.x == 0)
            {
                for (int y = min.y; y < max.y; y += ChunkSize)
                {
                    for (int z = min.z; z < max.z; z += ChunkSize)
                    {
                        glm::vec3 pos = getChunkpos(glm::vec3(0,y,z));
                        unsigned int key = getKeyVal(getHashVal(pos));
                        spatialLookup[objects.size() - 1 + count + chunkOffsets[i]] = std::make_pair(key, i);
                        startLookup[objects.size() - 1 + count + chunkOffsets[i]] = UINT32_MAX;
                        count++;
                    }
                }
            }
            else if(min.y == 0 && max.y == 0)
            {
                for (int x = min.x; x < max.x; x += ChunkSize)
                {
                    for (int z = min.z; z < max.z; z += ChunkSize)
                    {
                        glm::vec3 pos = getChunkpos(glm::vec3(x,0,z));
                        unsigned int key = getKeyVal(getHashVal(pos));
                        spatialLookup[objects.size() - 1 + count + chunkOffsets[i]] = std::make_pair(key, i);
                        startLookup[objects.size() - 1 + count + chunkOffsets[i]] = UINT32_MAX;
                        count++;
                    }
                }
            }
            else if(min.z == 0 && max.z == 0)
            {
                for (int x = min.x; x < max.x; x += ChunkSize)
                {
                    for (int y = min.y; y < max.y; y += ChunkSize)
                    {
                        glm::vec3 pos = getChunkpos(glm::vec3(x,y,0));
                        unsigned int key = getKeyVal(getHashVal(pos));
                        spatialLookup[objects.size() - 1 + count + chunkOffsets[i]] = std::make_pair(key, i);
                        startLookup[objects.size() - 1 + count + chunkOffsets[i]] = UINT32_MAX;
                        count++;
                    }
                }
            }
            else
            {
                for (int x = min.x; x < max.x; x += ChunkSize)
                {
                    for (int y = min.y; y < max.y; y += ChunkSize)
                    {
                        for (int z = min.z; z < max.z; z += ChunkSize)
                        {
                            std::cout << "test" << std::endl;
                            glm::vec3 pos = getChunkpos(glm::vec3(x,y,z));
                            unsigned int key = getKeyVal(getHashVal(pos));
                            spatialLookup[objects.size() - 1 + count + chunkOffsets[i]] = std::make_pair(key, i);
                            startLookup[objects.size() - 1 + count + chunkOffsets[i]] = UINT32_MAX;
                            count++;
                        }
                    }
                }
            }
        }
    }

    std::sort(spatialLookup.begin(), spatialLookup.end());

    for (unsigned int i = 0; i < startLookup.size(); i++)
    {
        unsigned int key = spatialLookup[i].first;
        unsigned int keyPrev;
        if(i == 0)
            keyPrev = 0;
        else
            keyPrev = spatialLookup[i - 1].first;
        if(key != keyPrev)
            startLookup[key] = i;
    }
    
    objectCount = objects.size();
}

glm::vec3 ChunkManager::getChunkpos(glm::vec3 pos)
{
    return glm::vec3((int)(pos.x / ChunkSize), (int)(pos.y / ChunkSize), (int)(pos.z / ChunkSize));
}

unsigned int ChunkManager::getHashVal(glm::vec3 pos)
{
    unsigned int a = pos.x * 12289;
    unsigned int b = pos.y * 786433;
    unsigned int c = pos.z * 50331652;
    return a + b + c;
}

unsigned int ChunkManager::getKeyVal(unsigned int hash)
{
    return hash % spatialLookup.size();
}