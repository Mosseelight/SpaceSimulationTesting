#include "../../include/Core/Physics/SpatialPhysics.hpp"

static ChunkManager cManager;

void RunSimulation(float deltaTime, Scene& scene, int runSim)
{

    //Big World Inititive
    /*
        chunk sleeping

        wake chunks
            for every object that is awake, wake its chunk it is in and the one it will be in (using velocity)

        sleeping chunks
            for every object that is awake, check if it is sleeping (compaare position from last frame to current and get length, 
            check for small value like 0.001f for small movements not visable)
            if all objects in a chunk are sleeping then the chunk is sleeping
        
        updating chunks
            if a object is sleeping dont update the chunk for it
            (best way is to remove it from the for loop check, worse way is to early check if it is sleeping)

        figure out a way to remove objects from updating if they are alseep
        (pass in the ids of the awak objects with the scene objects to run the update chunk method with those?)

    */

    static std::vector<unsigned int> ids;
    cManager.UpdateChunks(scene.SpatialObjects);
    for (unsigned int i = 0; i < scene.SpatialObjects.size(); i++)
    {
        ids = cManager.FindObjectsInChunk(scene.SpatialObjects, scene.SpatialObjects[i].SO_id);
        if(runSim == 1)
            scene.SpatialObjects[i].SO_rigidbody.Step(PhysicsStep * PhysicsSpeed, deltaTime, ids, scene.SpatialObjects, scene.SpatialObjects[i]);
        scene.SpatialObjects[i].SO_mesh.position = scene.SpatialObjects[i].SO_rigidbody.position;
        scene.SpatialObjects[i].SO_mesh.rotation = scene.SpatialObjects[i].SO_rigidbody.rotation;
        scene.SpatialObjects[i].SO_rigidbody.boundbox.ConstructBoundingBox(scene.SpatialObjects[i].SO_mesh);
        scene.SpatialObjects[i].SO_rigidbody.oriBoundBox.ConstructOriBoundingBox(scene.SpatialObjects[i].SO_mesh);
    }
}

void UpdateChunks(std::vector<SpatialObject>& SpatialObjects)
{
    cManager.UpdateChunks(SpatialObjects);
}