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
    unsigned int key = getKeyVal(getHashVal(pos));
    unsigned int startIndex = startLookup[key];

    for (unsigned int i = startIndex; i < spatialLookup.size(); i++)
    {
        unsigned int currentKey = std::get<0>(spatialLookup[i]);
        if (currentKey == key && objects[std::get<1>(spatialLookup[i])].SO_id != objectID)
            objectsInSameChunk.push_back(objects[std::get<1>(spatialLookup[i])].SO_id);
            
        else
            break;
    }

    //get objects in chunk that is ahead of the current object
    pos += glm::ceil(glm::normalize(objects[objectID].SO_rigidbody.velocity));
    //object velocity points to same chunk so dont readd the objects
    if(key == getKeyVal(getHashVal(pos)))
        return objectsInSameChunk;
    key = getKeyVal(getHashVal(pos));
    startIndex = startLookup[key];

    for (unsigned int i = startIndex; i < spatialLookup.size(); i++)
    {
        unsigned int currentKey = std::get<0>(spatialLookup[i]);
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
        chunkOffsets.resize(objects.size() + 1);
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
                    spatialLookup.push_back(std::make_tuple(0, 0, false));
                    startLookup.push_back(0);
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
        if(glm::distance(objects[i].SO_rigidbody.boundbox.max, objects[i].SO_rigidbody.boundbox.min) / ChunkSize < 1)
        {
            glm::vec3 pos = getChunkpos(objects[i].SO_rigidbody.position);
            unsigned int key = getKeyVal(getHashVal(pos));
            spatialLookup[i] = std::make_tuple(key, i, false);
            startLookup[i] = UINT32_MAX;
        }
        else
        {
            glm::vec3 min = objects[i].SO_rigidbody.boundbox.min;
            glm::vec3 max = objects[i].SO_rigidbody.boundbox.max;
            unsigned int count = 0;

            for (float x = min.x; x < max.x + 0.0001f; x += ChunkSize)
            {
                for (float y = min.y; y < max.y + 0.0001f; y += ChunkSize)
                {
                    for (float z = min.z; z < max.z + 0.0001f; z += ChunkSize)
                    {
                        glm::vec3 pos = getChunkpos(glm::vec3(x,y,z));
                        unsigned int key = getKeyVal(getHashVal(pos));
                        spatialLookup[objects.size() - 1 + count + chunkOffsets[i]] = std::make_tuple(key, i, objects[i].SO_rigidbody.isAwake);
                        startLookup[objects.size() - 1 + count + chunkOffsets[i]] = UINT32_MAX;
                        count++;
                    }
                }
            }
        }
    }

    std::sort(spatialLookup.begin(), spatialLookup.end());

    for (unsigned int i = 0; i < startLookup.size(); i++)
    {
        unsigned int key = std::get<0>(spatialLookup[i]);
        unsigned int keyPrev;
        if(i == 0)
            keyPrev = 0;
        else
            keyPrev = std::get<0>(spatialLookup[i - 1]);
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