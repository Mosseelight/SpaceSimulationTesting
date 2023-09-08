#include "../../include/Core/Physics/RigidBody.hpp"

void Solver(glm::vec3& out, glm::vec3 in, float step);

BoundingBox::BoundingBox()
{
    min = glm::vec3(0);
    max = glm::vec3(0);
}

BoundingBox::~BoundingBox()
{

}

void BoundingBox::ConstructBoundingBox(Mesh& mesh)
{
    min = glm::vec3(FLT_MAX);
    max = glm::vec3(FLT_MIN);
    for (unsigned int i = 0; i < mesh.vertexes.size(); i++)
    {
        if(mesh.vertexes[i].position.x < min.x && mesh.vertexes[i].position.y < min.y && mesh.vertexes[i].position.z < min.z)
            min = mesh.vertexes[i].position;
        if(mesh.vertexes[i].position.x > max.x && mesh.vertexes[i].position.y > max.y && mesh.vertexes[i].position.z > max.z)
            max = mesh.vertexes[i].position;
    }
}



RigidBody::RigidBody()
{
    mass = 1.0f;
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    rotVelocity = glm::vec3(0.0f);
    rotAcceleration = glm::vec3(0.0f);
    totalForce = glm::vec3(0.0f);
}

RigidBody::~RigidBody()
{

}

void RigidBody::Step(float timeStep)
{
    totalForce = glm::vec3(0);
    totalRotation = glm::vec3(0);

    //APPLY FORCE HERE
    ApplyRotationForce(glm::vec3(0,10000.0f,0));

    acceleration = totalForce / mass;
    rotAcceleration = totalRotation / mass;
    Solver(velocity, acceleration * timeStep, 1.0f);
    Solver(rotVelocity, rotAcceleration * timeStep, 1.0f);
    Solver(rotation, rotVelocity * timeStep, 1.0f);
    Solver(position, velocity * timeStep, 1.0f);
}

void RigidBody::ApplyForce(glm::vec3 force)
{
    totalForce += force;
}

void RigidBody::ApplyForceAtPos(glm::vec3 force, glm::vec3 pos)
{
    totalForce += force;
    ApplyRotationForce(glm::cross(pos, force));
}

void RigidBody::ApplyImpulseForce(glm::vec3 dir, float power)
{
    velocity += glm::normalize(dir) * power;
}

void RigidBody::ApplyDragForce(float airDensity, float area)
{
    totalForce += 0.5f * airDensity * area * -velocity;
}

void RigidBody::ApplyRotationForce(glm::vec3 force)
{
    totalRotation += force;
}

void RigidBody::ApplyRotationImpulseForce(glm::vec3 dir, float power)
{
    rotVelocity += glm::normalize(dir) * power;
}

glm::vec3 RigidBody::GetLocalDir(glm::vec3 dir)
{
    glm::mat4 rotMat = glm::mat4(1.0f);
    rotMat = glm::rotate(rotMat, rotation.x * M_PIf/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.y * M_PIf/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.z * M_PIf/180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    return rotMat * glm::vec4(dir, 1.0f);
}

void Solver(glm::vec3& out, glm::vec3 in, float step)
{
    glm::vec3 temp1, temp2, temp3, temp4 = glm::vec3(0);
    temp1 = step * in;
    temp2 = step * glm::vec3(temp1.x * (step * 0.5), temp1.y * (step * 0.5), temp1.z * (step * 0.5));
    temp3 = step * glm::vec3(temp2.x * (step * 0.5), temp2.y * (step * 0.5), temp2.z * (step * 0.5));
    temp4 = step * glm::vec3(temp3.x * (step * 0.5), temp3.y * (step * 0.5), temp3.z * (step * 0.5));
    out += (1.0f / 6.0f) * (temp1 + 2.0f * temp2 + 2.0f * temp3 + temp4);
}