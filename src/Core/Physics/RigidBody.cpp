#include "../../include/Core/Physics/RigidBody.hpp"

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
    totalForce = glm::vec3(0);
    ApplyForce(glm::vec3(0,-9.81f,0));
    ApplyDragForce(AirDensity, 1.0f);
    acceleration = totalForce / mass;
    velocity += acceleration * timeStep;
    position += velocity * timeStep;
}

void RigidBody::ApplyForce(glm::vec3 force)
{
    totalForce += force * mass;
}

void RigidBody::ApplyDragForce(float airDensity, float area)
{
    totalForce += 0.5f * airDensity * area * -velocity;
}

glm::vec3 RigidBody::GetLocalDir(glm::vec3 dir)
{
    glm::mat4 rotMat = glm::mat4(1.0f);
    rotMat = glm::rotate(rotMat, rotation.x * M_PIf/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.y * M_PIf/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.z * M_PIf/180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    return rotMat * glm::vec4(dir, 1.0f);
}