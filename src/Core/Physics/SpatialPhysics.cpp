#include "../../include/Core/Physics/SpatialPhysics.hpp"

void RunSimulation(float deltaTime, Scene& scene)
{
    static std::vector<unsigned int> ids;
    static float totalTime;
    static ChunkManager cManager;
    int counter = 0;
    totalTime += deltaTime;
    while (totalTime >= PhysicsStep)
    {
        //go through all spatials and update
        cManager.UpdateChunks(scene.SpatialObjects);
        for (unsigned int i = 0; i < scene.SpatialObjects.size(); i++)
        {
            ids = cManager.FindObjectsInChunk(scene.SpatialObjects, scene.SpatialObjects[i].SO_id);
            scene.SpatialObjects[i].SO_rigidbody.Step(PhysicsStep * PhysicsSpeed, ids, scene.SpatialObjects, scene.SpatialObjects[i]);
            scene.SpatialObjects[i].SO_mesh.position = scene.SpatialObjects[i].SO_rigidbody.position;
            scene.SpatialObjects[i].SO_mesh.rotation = scene.SpatialObjects[i].SO_rigidbody.rotation;
            scene.SpatialObjects[i].SO_rigidbody.boundbox.ConstructBoundingBox(scene.SpatialObjects[i].SO_mesh);
            scene.SpatialObjects[i].SO_rigidbody.ooBoundBox.ConstructOrientiedBox(scene.SpatialObjects[i]);
            DrawDebugCube(scene.SpatialObjects[i].SO_rigidbody.ooBoundBox.max, 0.1f, glm::vec3(255,0,0));
            DrawDebugCube(scene.SpatialObjects[i].SO_rigidbody.ooBoundBox.min, 0.1f, glm::vec3(0,0,255));
        }
        counter++;

        if(deltaTime >= PhysicsStep)
        {
            if(counter == maxPhysicSteps)
                break;
            totalTime -= PhysicsStep;
        }
        else
        {
            totalTime = 0;
            break;
        }
    }
}