#include "../../include/Core/Physics/SpatialPhysics.hpp"

void RunSimulation(float deltaTime, Scene& scene)
{
    static float totalTime;
    totalTime += deltaTime;
    while (totalTime >= PhysicsStep)
    {
        //go through all spatials and update
        for (unsigned int i = 0; i < scene.SpatialObjects.size(); i++)
        {
            scene.SpatialObjects[i].SO_rigidbody.Step(PhysicsStep * PhysicsSpeed, scene.SpatialObjects[0], scene.SpatialObjects[1]);
            scene.SpatialObjects[i].SO_mesh.position = scene.SpatialObjects[i].SO_rigidbody.position;
            scene.SpatialObjects[i].SO_mesh.rotation = scene.SpatialObjects[i].SO_rigidbody.rotation;
            scene.SpatialObjects[i].SO_rigidbody.boundbox.ConstructBoundingBox(scene.SpatialObjects[i].SO_mesh);
        }
        totalTime -= PhysicsStep;
    }
}