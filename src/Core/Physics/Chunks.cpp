#include "../../include/Core/Physics/Chunks.hpp"

Chunk::Chunk()
{
    min = glm::vec3(0);
    max = glm::vec3(0);
    chunkObject = NULL;
    TLChunk = NULL;
    TRChunk = NULL;
    BLChunk = NULL;
    BRChunk = NULL;

}

Chunk::Chunk(glm::vec3 min, glm::vec3 max)
{
    this->min = min;
    this->max = max;
    chunkObject = NULL;
    TLChunk = NULL;
    TRChunk = NULL;
    BLChunk = NULL;
    BRChunk = NULL;
}

Chunk::~Chunk()
{
    if (TLChunk != nullptr) 
    {
        delete TLChunk;
        TLChunk = nullptr;
    }
    if (TRChunk != nullptr) 
    {
        delete TRChunk;
        TRChunk = nullptr;
    }
    if (BLChunk != nullptr) 
    {
        delete BLChunk;
        BLChunk = nullptr;
    }
    if (BRChunk != nullptr) 
    {
        delete BRChunk;
        BRChunk = nullptr;
    }
}

void Chunk::InsertChunk(SpatialObject& object)
{

    if (!inChunk(object.SO_rigidbody.position))
        return;
 
    if (abs(max.x - min.x) <= 1 && abs(max.y - min.y) <= 1) 
    {
        if (chunkObject == NULL)
            chunkObject = &object;
        return;
    }
 
    if ((max.x + min.x) / 2 >= object.SO_rigidbody.position.x && (max.z + min.z) / 2 >= object.SO_rigidbody.position.z) 
    {
        //topleft
        if ((max.y + min.y) / 2 >= object.SO_rigidbody.position.y) 
        {
            if (TLChunk == NULL)
                TLChunk = new Chunk(glm::vec3(max.x, max.y, max.z), glm::vec3((max.x + min.x) / 2, (max.y + min.y) / 2, (max.z + max.z) / 2));
            TLChunk->InsertChunk(object);
        }
 
        //bottemleft
        else 
        {
            if (BLChunk == NULL)
                BLChunk = new Chunk(glm::vec3(max.x, (max.y + min.y) / 2, max.z), glm::vec3((max.x + min.x) / 2, min.y, (max.z + max.z) / 2));
            BLChunk->InsertChunk(object);
        }
    }
    else if((max.z + min.z) / 2 >= object.SO_rigidbody.position.z) 
    {
        //topright
        if ((max.y + min.y) / 2 >= object.SO_rigidbody.position.y) 
        {
            if (TRChunk == NULL)
                TRChunk = new Chunk(glm::vec3((max.x + min.x) / 2, max.y, max.z), glm::vec3(min.x, (max.y + min.y) / 2, (max.z + max.z) / 2));
            TRChunk->InsertChunk(object);
        }
 
        //bottomright
        else 
        {
            if (BRChunk == NULL)
                BRChunk = new Chunk(glm::vec3((max.x + min.x) / 2, (max.y + min.y) / 2, max.z), glm::vec3(min.x, min.y, (max.z + max.z) / 2));
            BRChunk->InsertChunk(object);
        }
    }
}

SpatialObject* Chunk::SearchChunk(glm::vec3 pos)
{
    if (!inChunk(pos))
        return NULL;
 
    if (chunkObject != NULL)
        return chunkObject;
 
    if ((max.x + min.x) / 2 >= pos.x) {
        //topleft
        if ((max.y + min.y) / 2 >= pos.y) {
            if (TLChunk == NULL)
                return NULL;
            return TLChunk->SearchChunk(pos);
        }
 
        //bottomleft
        else {
            if (BLChunk == NULL)
                return NULL;
            return BLChunk->SearchChunk(pos);
        }
    }
    else {
        //topright
        if ((max.y + min.y) / 2 >= pos.y) {
            if (TRChunk == NULL)
                return NULL;
            return TRChunk->SearchChunk(pos);
        }
 
        //bottomright
        else {
            if (BRChunk == NULL)
                return NULL;
            return BRChunk->SearchChunk(pos);
        }
    }
}

bool Chunk::inChunk(glm::vec3 pos)
{
    return (pos.x >= max.x && pos.x <= min.x && pos.y >= max.y && pos.y <= min.y);
}