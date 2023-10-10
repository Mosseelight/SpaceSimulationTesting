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
    std::vector<unsigned int> objectsInSameChunk;
    glm::vec3 pos = getChunkpos(objects[objectID].SO_rigidbody.position);
    unsigned int key = getKayVal(getHashVal(pos));

    for (unsigned int i = 0; i < objects.size(); i++)
    {
        unsigned int currentKey = spatialLookup[i].first;
        if (currentKey != key)
            break;

        objectsInSameChunk.push_back(objects[spatialLookup[i].second].SO_id);
    }
    return objectsInSameChunk;
}

void ChunkManager::UpdateChunks(std::vector<SpatialObject>& objects)
{
    if(spatialLookup.size() == 0)
    {
        spatialLookup.resize(objects.size());
    }

    for (unsigned int i = 0; i < objects.size(); i++)
    {
        //start at object max and step 10 to min until euqal or greater
        //get pos at each step and get hash and key
        // add to spatial lookup with the same id

        glm::vec3 pos = getChunkpos(objects[i].SO_rigidbody.position);
        unsigned int key = getKayVal(getHashVal(pos));
        spatialLookup[i] = std::make_pair(key, i);
        startLookup.push_back(4294967295);
    }

    std::sort(spatialLookup.begin(), spatialLookup.end());
    
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        unsigned int key = spatialLookup[i].first;
        unsigned int keyPrev;
        if(i == 0)
            keyPrev = 4294967295;
        else
            keyPrev = spatialLookup[i - 1].first;
        if(key != keyPrev)
            startLookup[key] = i;
    }
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

unsigned int ChunkManager::getKayVal(unsigned int hash)
{
    return hash % spatialLookup.size();
}