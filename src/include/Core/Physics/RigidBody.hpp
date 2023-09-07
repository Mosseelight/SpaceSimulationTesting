#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Globals.hpp"

class RigidBody
{
private:
    
public:

    float mass;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 rotation;

    glm::vec3 totalForce;

    RigidBody();
    ~RigidBody();
    void Step(float timeStep);
    void ApplyForce(glm::vec3 force);
    void ApplyDragForce(float airDensity, float area);
    glm::vec3 GetLocalDir(glm::vec3 dir);
};
