#include "../../include/Core/Physics/Chunks.hpp"

ChunkManager::ChunkManager()
{
    ChunkSize = 10;
}

ChunkManager::~ChunkManager()
{

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
        spatialLookup.push_back(std::make_pair(key, i));
        startLookup.push_back(4294967295);
    }
    
    std::sort(spatialLookup.begin(), spatialLookup.end());

    for (size_t i = spatialLookup.size() / 2 - 1; i < spatialLookup.size(); i++)
    {
        std::cout << spatialLookup[i].first << " " << spatialLookup[i].second << " " << i << std::endl;
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
    unsigned int c = pos.z * 50331653;
    return a + b + c;
}

unsigned int ChunkManager::getKayVal(unsigned int hash)
{
    return hash % spatialLookup.size();
}