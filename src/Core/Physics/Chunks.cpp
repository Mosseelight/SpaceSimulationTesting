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
    glm::vec3 pos = getChunkpos(objects[objectID].SO_rigidbody.boundbox.max);
    int key = getKeyVal(getHashVal(pos));
    unsigned int startIndex = startLookup[key];

    // for any objects that span multiple chunks we need to add to the list of their keys plus also any objects
    // that border chunks
    glm::vec3 min = objects[objectID].SO_rigidbody.boundbox.min;
    glm::vec3 max = objects[objectID].SO_rigidbody.boundbox.max;
    for (float x = min.x; x < max.x + 0.0001f; x += ChunkSize)
    {
        for (float y = min.y; y < max.y + 0.0001f; y += ChunkSize)
        {
            for (float z = min.z; z < max.z + 0.0001f; z += ChunkSize)
            {
                pos = getChunkpos(glm::vec3(x,y,z));
                key = getKeyVal(getHashVal(pos));
                startIndex = startLookup[key];
                for (unsigned int i = startIndex; i < spatialLookup.size(); i++)
                {
                    int currentKey = std::get<0>(spatialLookup[i]);
                    if (currentKey == key && std::get<1>(spatialLookup[i]) != objectID)
                    {
                        objectsInSameChunk.push_back(std::get<1>(spatialLookup[i]));
                    }
                        
                    else
                        break;
                }
            }
        }
    }

    // do the max position as it is not accounted in the for loop
    if(key != getKeyVal(getHashVal(max)))
    {
        pos = getChunkpos(max);
        key = getKeyVal(getHashVal(pos));
        startIndex = startLookup[key];
        for (unsigned int i = startIndex; i < spatialLookup.size(); i++)
        {
            int currentKey = std::get<0>(spatialLookup[i]);
            if (currentKey == key && std::get<1>(spatialLookup[i]) != objectID)
            {
                objectsInSameChunk.push_back(std::get<1>(spatialLookup[i]));
            }
                
            else
                break;
        }
    }

    pos += objects[objectID].SO_rigidbody.velocity;
    //object velocity points to same chunk so dont readd the objects
    if(key == getKeyVal(getHashVal(pos)))
        return objectsInSameChunk;
    key = getKeyVal(getHashVal(pos));
    startIndex = startLookup[key];

    for (unsigned int i = startIndex; i < spatialLookup.size(); i++)
    {
        int currentKey = std::get<0>(spatialLookup[i]);
        if (currentKey == key && objects[std::get<1>(spatialLookup[i])].SO_id != objectID)
            objectsInSameChunk.push_back(objects[std::get<1>(spatialLookup[i])].SO_id);
        else
            break;
    }

    return objectsInSameChunk;
}

void ChunkManager::UpdateChunks(std::vector<SpatialObject>& objects)
{
    static unsigned int objectCount = 0;

    //array space allocation to fit any new objects or objects that span multiple chunks
    //have to span the max amount possible that it can which is highest 
    if(objectCount != objects.size())
    {
        spatialLookup.resize(objects.size());
        startLookup.clear();
        startLookup.resize(objects.size());
        chunkOffsets.resize(objects.size() + 2);
        unsigned int spanCount = 0;
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            unsigned int offset = 0;
            if(glm::distance(objects[i].SO_rigidbody.oriBoundBox.maxOri, objects[i].SO_rigidbody.oriBoundBox.minOri) / ChunkSize > 1)
            {
                spanCount++;
                glm::vec3 min = objects[i].SO_rigidbody.oriBoundBox.minOri;
                glm::vec3 max = objects[i].SO_rigidbody.oriBoundBox.maxOri;

                //the maxiumum possible extents a object can have if it spans chunks so that if it is rotated quickly
                //a seg fault does not occur
                float maxL = fmaxf(max.x - min.x, fmaxf(max.y - min.y, max.z - min.z)) / ChunkSize;
                
                for (size_t i = 0; i < maxL * maxL * maxL; i++)
                {
                    spatialLookup.push_back(std::make_tuple(INT_MAX, INT_MAX, false));
                    startLookup.push_back(UINT_MAX);
                    offset++;
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
        //static std::pair<int, unsigned int> keyBefore = std::make_pair(0, 0);
        //unsigned int index = 0;
        if(glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / ChunkSize < 1)
        {
            glm::vec3 pos = getChunkpos(objects[i].SO_rigidbody.position);
            int key = getKeyVal(getHashVal(pos));
            /*for (unsigned int g = 0; g < spatialLookup.size(); g++)
            {
                if(i == 0)
                {
                    spatialLookup.insert(spatialLookup.begin(), std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake));
                    spatialLookup.erase(spatialLookup.end());
                    index = 0;
                    break;
                }
                if(key == keyBefore.first)
                {
                    spatialLookup.insert(spatialLookup.begin() + keyBefore.second + 1, std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake));
                    spatialLookup.erase(spatialLookup.end());
                    index = keyBefore.second + 1;
                    break;
                }
                if(key > keyBefore.first)
                {
                    if(keyBefore.second + 1 == spatialLookup.size())
                    {
                        spatialLookup.insert(spatialLookup.begin() + keyBefore.second, std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake));
                        spatialLookup.erase(spatialLookup.end());
                        index = keyBefore.second;
                        break;
                    }
                    if(std::get<0>(spatialLookup[keyBefore.second + 1]) < key)
                    {
                        keyBefore.first = std::get<0>(spatialLookup[keyBefore.second + 1]);
                        keyBefore.second = keyBefore.second + 1;
                    }
                    else
                    {
                        spatialLookup.insert(spatialLookup.begin() + keyBefore.second + 1, std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake));
                        spatialLookup.erase(spatialLookup.end());
                        index = keyBefore.second + 1;
                        break;
                    }
                }
                else
                {
                    if(keyBefore.second - 1 == -1)
                    {
                        spatialLookup.insert(spatialLookup.begin(), std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake));
                        spatialLookup.erase(spatialLookup.end());
                        index = 0;
                        break;
                    }
                    if(std::get<0>(spatialLookup[keyBefore.second - 1]) > key)
                    {
                        keyBefore.first = std::get<0>(spatialLookup[keyBefore.second - 1]);
                        keyBefore.second = keyBefore.second - 1;
                    }
                    else
                    {
                        spatialLookup.insert(spatialLookup.begin() + keyBefore.second - 1, std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake));
                        spatialLookup.erase(spatialLookup.end());
                        index = keyBefore.second - 1;
                        break;
                    }
                }
            }*/
            spatialLookup[i] = std::make_tuple(key, i, false);
            startLookup[i] = UINT32_MAX;
        }
        else
        {
            glm::vec3 min = objects[i].SO_rigidbody.oriBoundBox.min;
            glm::vec3 max = objects[i].SO_rigidbody.oriBoundBox.max;
            unsigned int count = 0;

            for (float x = min.x; x < max.x + 0.0001f; x += ChunkSize)
            {
                for (float y = min.y; y < max.y + 0.0001f; y += ChunkSize)
                {
                    for (float z = min.z; z < max.z + 0.0001f; z += ChunkSize)
                    {
                        glm::vec3 pos = getChunkpos(glm::vec3(x,y,z));
                        int key = getKeyVal(getHashVal(pos));
                        spatialLookup[objects.size() - 1 + count + chunkOffsets[i]] = std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake);
                        startLookup[objects.size() - 1 + count + chunkOffsets[i]] = UINT32_MAX;
                        count++;
                    }
                }
            }
        }
        //keyBefore.first = std::get<0>(spatialLookup[index]);
        //keyBefore.second = index;
    }

    std::sort(spatialLookup.begin(), spatialLookup.end());

    for (unsigned int i = 0; i < startLookup.size(); i++)
    {
        int key = std::get<0>(spatialLookup[i]);
        int keyPrev;
        if(i == 0)
            keyPrev = 0;
        else
            keyPrev = std::get<0>(spatialLookup[i - 1]);
        if(key == INT_MAX)
            key = keyPrev;
        if(key != keyPrev)
            startLookup[key] = i;
    }

    objectCount = objects.size();
}

glm::vec3 ChunkManager::getChunkpos(glm::vec3 pos)
{
    return glm::vec3((int)(pos.x / ChunkSize), (int)(pos.y / ChunkSize), (int)(pos.z / ChunkSize));
}

int ChunkManager::getHashVal(glm::vec3 pos)
{
    int a = pos.x * 12289;
    int b = pos.y * 786433;
    int c = pos.z * 50331652;
    return a ^ b ^ c;
}

int ChunkManager::getKeyVal(int hash)
{
    return hash % spatialLookup.size();
}