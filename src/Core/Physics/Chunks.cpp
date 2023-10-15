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
    //use the start lookup array
    std::vector<unsigned int> objectsInSameChunk;
    glm::vec3 pos = getChunkpos(objects[objectID].SO_rigidbody.position);
    unsigned int key = getKeyVal(getHashVal(pos));

    for (unsigned int i = 0; i < spatialLookup.size(); i++)
    {
        unsigned int currentKey = spatialLookup[i].first;
        if (currentKey == key && objects[spatialLookup[i].second].SO_id != objectID)
            objectsInSameChunk.push_back(objects[spatialLookup[i].second].SO_id);
    }

    return objectsInSameChunk;
}

void ChunkManager::UpdateChunks(std::vector<SpatialObject>& objects)
{
    if(spatialLookup.size() == 0)
    {
        spatialLookup.resize(objects.size());
        unsigned int totalSpan = 0;
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            if(glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / ChunkSize > 1)
            {
                glm::vec3 min = objects[i].SO_rigidbody.boundbox.min;
                glm::vec3 max = objects[i].SO_rigidbody.boundbox.max;
                for (int x = min.x; x < max.x; x += ChunkSize)
                {
                    for (int y = min.y; y < max.y; y += ChunkSize)
                    {
                        for (int z = min.z; z < max.z; z += ChunkSize)
                        {
                            spatialLookup.push_back(std::make_pair(0, 0));
                            //startLookup.push_back(4294967295);
                        }
                    }
                }
                //spatialLookup.pop_back();
            }
        }
    }

    for (unsigned int i = 0; i < objects.size(); i++)
    {
        if(glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / ChunkSize < 1)
        {
            glm::vec3 pos = getChunkpos(objects[i].SO_rigidbody.position);
            unsigned int key = getKeyVal(getHashVal(pos));
            spatialLookup[i] = std::make_pair(key, i);
            //startLookup[i] = 4294967295;
        }
        else
        {
            glm::vec3 min = objects[i].SO_rigidbody.boundbox.min;
            glm::vec3 max = objects[i].SO_rigidbody.boundbox.max;
            unsigned int count = 0;
            for (int x = min.x; x < max.x; x += ChunkSize)
            {
                for (int y = min.y; y < max.y; y += ChunkSize)
                {
                    for (int z = min.z; z < max.z; z += ChunkSize)
                    {
                        //newPos = glm::vec3(x,y,z);
                        glm::vec3 pos = getChunkpos(glm::vec3(x,y,z));
                        unsigned int key = getKeyVal(getHashVal(pos));
                        spatialLookup[objects.size() - 1 + count] = std::make_pair(key, i);
                        //startLookup[objects.size() - 1 + count] = 4294967295;
                        count++;
                    }
                }
            }
        }
    }

    std::sort(spatialLookup.begin(), spatialLookup.end());
    
    /*for (unsigned int i = 0; i < objects.size(); i++)
    {
        unsigned int key = spatialLookup[i].first;
        unsigned int keyPrev;
        if(i == 0)
            keyPrev = 4294967295;
        else
            keyPrev = spatialLookup[i - 1].first;
        if(key != keyPrev)
            startLookup[key] = i;
    }*/
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