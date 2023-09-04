#pragma once
#include <glm/glm.hpp>

class RigidBody
{
private:
    
public:

    glm::vec3 position;
    glm::vec3 velocity;

    RigidBody();
    ~RigidBody();
    void Step(double timeStep);
};
