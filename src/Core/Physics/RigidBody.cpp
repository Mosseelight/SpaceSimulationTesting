#include "../../include/Core/Physics/RigidBody.hpp"
#include <iostream>

RigidBody::RigidBody()
{
    mass = 1.0f;
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    totalForce = glm::vec3(0.0f);
}

RigidBody::~RigidBody()
{

}

void RigidBody::Step(float timeStep)
{
    AddForce(GetLocalDir(glm::vec3(0,-1.0f,0)));
    acceleration = totalForce / mass;
    velocity += acceleration * timeStep;
    position += velocity * timeStep;
}

void RigidBody::AddForce(glm::vec3 force)
{
    totalForce += force;
}

glm::vec3 RigidBody::GetLocalDir(glm::vec3 dir)
{
    glm::mat4 rotMat = glm::mat4(1.0f);
    rotMat = glm::rotate(rotMat, rotation.x * M_PIf/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.y * M_PIf/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.z * M_PIf/180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    return rotMat * glm::vec4(dir, 1.0f);
}