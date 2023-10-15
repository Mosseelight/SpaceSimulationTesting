#include "../../include/Core/Physics/SpatialPhysics.hpp"

void RunSimulation(float deltaTime, Scene& scene)
{
    static float totalTime;
    static ChunkManager cManager;
    int counter = 0;
    totalTime += deltaTime;
    while (totalTime >= PhysicsStep)
    {
        if(counter > maxPhysicSteps)
            break;
        //go through all spatials and update
        cManager.UpdateChunks(scene.SpatialObjects);
        for (unsigned int i = 0; i < scene.SpatialObjects.size(); i++)
        {
            scene.SpatialObjects[i].SO_rigidbody.Step(PhysicsStep * PhysicsSpeed, cManager.FindObjectsInChunk(scene.SpatialObjects, scene.SpatialObjects[i].SO_id), scene.SpatialObjects, scene.SpatialObjects[i]);
            scene.SpatialObjects[i].SO_mesh.position = scene.SpatialObjects[i].SO_rigidbody.position;
            scene.SpatialObjects[i].SO_mesh.rotation = scene.SpatialObjects[i].SO_rigidbody.rotation;
            scene.SpatialObjects[i].SO_rigidbody.boundbox.ConstructBoundingBox(scene.SpatialObjects[i].SO_mesh);
        }
        counter++;
        totalTime -= PhysicsStep;
    }
}