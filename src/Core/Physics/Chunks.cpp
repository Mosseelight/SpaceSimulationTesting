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

    for (unsigned int i = 0; i < spatialLookup.size(); i++)
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
        //create spots for an object that covers multiple chunks
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            if(!glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / 10.0f < 1)
            {
                glm::vec3 newPos = objects[i].SO_rigidbody.boundbox.max;
                glm::vec3 min = objects[i].SO_rigidbody.boundbox.min;
                while (newPos.x >= min.x && newPos.y >= min.y && newPos.z >= min.z)
                {
                    spatialLookup.insert(spatialLookup.begin() + i, std::make_pair(0, 0));
                    startLookup.push_back(4294967295);
                    newPos -= glm::normalize(newPos - min) * 10.0f;
                }
            }
        }
    }

    for (unsigned int i = 0; i < objects.size(); i++)
    {
        //--FIX--
        // if a object only has one chunk then it needs to start at its span 
        // have an array of values that are the span of chunk it covers with the id?
        // at object 0 it says this goes over 14 chunks so next object starts at 14 index
        //-------
        // object will only span across one chunk
        if(glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / 10.0f < 1)
        {
            glm::vec3 pos = getChunkpos(objects[i].SO_rigidbody.position);
            unsigned int key = getKayVal(getHashVal(pos));
            spatialLookup[i] = std::make_pair(key, i);
            std::cout << i << std::endl;
            startLookup.push_back(4294967295);
            DrawDebugCube(objects[i].SO_rigidbody.position, 0.4f, glm::vec3(0,0,255));
        }
        // object spans across multiple chunks
        else
        {
            glm::vec3 newPos = objects[i].SO_rigidbody.boundbox.max;
            glm::vec3 min = objects[i].SO_rigidbody.boundbox.min;
            unsigned int count = glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / 10.0f;
            for (unsigned int g = 0; g < count; g++)
            {
                glm::vec3 pos = getChunkpos(newPos);
                unsigned int key = getKayVal(getHashVal(pos));
                spatialLookup[i] = std::make_pair(key, i + g);
                //std::cout << i + g << std::endl;
                startLookup.push_back(4294967295);
                newPos -= glm::normalize(newPos - min) * 10.0f;
                DrawDebugCube(newPos, 0.4f, glm::vec3(0,0,255));
            }
        }
        
        //start at object max and step 10 to min until euqal or greater
        //get pos at each step and get hash and key
        // add to spatial lookup with the same id
    }

    std::sort(spatialLookup.begin(), spatialLookup.end());

    for (size_t i = 0; i < spatialLookup.size(); i++)
    {
        //std::cout << spatialLookup[i].first << " " << spatialLookup[i].second << std::endl;
    }
    
    
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