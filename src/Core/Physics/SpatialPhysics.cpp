#include "../../include/Core/Physics/SpatialPhysics.hpp"

void RunSimulation(float deltaTime, Scene& scene)
{
    static float totalTime;
    int counter = 0;
    totalTime += deltaTime;
    while (totalTime >= PhysicsStep)
    {
        if(counter > maxPhysicSteps)
            break;
        //go through all spatials and update
        for (unsigned int i = 0; i < scene.SpatialObjects.size(); i++)
        {
            scene.SpatialObjects[i].SO_rigidbody.Step(PhysicsStep * PhysicsSpeed, scene.SpatialObjects, scene.SpatialObjects[i]);
            scene.SpatialObjects[i].SO_mesh.position = scene.SpatialObjects[i].SO_rigidbody.position;
            scene.SpatialObjects[i].SO_mesh.rotation = scene.SpatialObjects[i].SO_rigidbody.rotation;
            scene.SpatialObjects[i].SO_rigidbody.boundbox.ConstructBoundingBox(scene.SpatialObjects[i].SO_mesh);
        }
        counter++;
        totalTime -= PhysicsStep;
    }
}