#include "../../include/Core/Physics/SpatialPhysics.hpp"

void RunSimulation(float deltaTime, Scene& scene, int runSim)
{
    static std::vector<unsigned int> ids;
    static ChunkManager cManager;
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