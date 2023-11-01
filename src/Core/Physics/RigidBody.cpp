#include "../../include/Core/Physics/RigidBody.hpp"
#include "../../include/Core/Physics/Collision.hpp"
#include "../../include/Core/Scene.hpp"
#include "../../include/Core/Math.hpp"

void Solver(glm::vec3& out, glm::vec3 in, float step);

BoundingBox::BoundingBox()
{
    max = glm::vec3(0);
    min = glm::vec3(0);
    maxOri = glm::vec3(0);
    minOri = glm::vec3(0);
    foundBox = false;
}


void BoundingBox::ConstructBoundingBox(Mesh& mesh)
{
    glm::vec3 mintmp = glm::vec3(FLT_MAX);
    glm::vec3 maxtmp = glm::vec3(FLT_MIN);
    for (unsigned int i = 0; i < mesh.vertexes.size(); i++)
    {
        glm::vec3 vertexPos = TransformVec4(glm::vec4(mesh.vertexes[i].position, 1.0f), mesh.modelMatrix);

        if (vertexPos.x < mintmp.x)
            mintmp.x = vertexPos.x;
        if (vertexPos.y < mintmp.y)
            mintmp.y = vertexPos.y;
        if (vertexPos.z < mintmp.z)
            mintmp.z = vertexPos.z;

        if (vertexPos.x > maxtmp.x)
            maxtmp.x = vertexPos.x;
        if (vertexPos.y > maxtmp.y)
            maxtmp.y = vertexPos.y;
        if (vertexPos.z > maxtmp.z)
            maxtmp.z = vertexPos.z;
    }
    
    min = mintmp;
    max = maxtmp;
}

void BoundingBox::ConstructOriBoundingBox(Mesh& mesh)
{
    if(!foundBox)
    {
        glm::vec3 mintmp = glm::vec3(FLT_MAX);
        glm::vec3 maxtmp = glm::vec3(FLT_MIN);
        for (unsigned int i = 0; i < mesh.vertexes.size(); i++)
        {
            glm::vec3 vertexPos = mesh.vertexes[i].position;

            if (vertexPos.x < mintmp.x)
                mintmp.x = vertexPos.x;
            if (vertexPos.y < mintmp.y)
                mintmp.y = vertexPos.y;
            if (vertexPos.z < mintmp.z)
                mintmp.z = vertexPos.z;

            if (vertexPos.x > maxtmp.x)
                maxtmp.x = vertexPos.x;
            if (vertexPos.y > maxtmp.y)
                maxtmp.y = vertexPos.y;
            if (vertexPos.z > maxtmp.z)
                maxtmp.z = vertexPos.z;
        }

        minOri = mintmp;
        maxOri = maxtmp;
        foundBox = true;
    }

    min = TransformVec4(glm::vec4(minOri, 1.0f), mesh.modelMatrix);
    max = TransformVec4(glm::vec4(maxOri, 1.0f), mesh.modelMatrix);

    if(min.x > max.x || min.y > max.y || min.z > max.z)
    {
        glm::vec3 tmp = max;
        max = min;
        min = tmp;
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
    isStatic = false;
    gForce = 0;
}

RigidBody::~RigidBody()
{

}

bool once = true;
void RigidBody::Step(float timeStep, float deltaTime, std::vector<unsigned int>& objectIds, std::vector<SpatialObject>& objects, SpatialObject& own)
{
    if(isStatic)
        return;
    totalForce = glm::vec3(0);
    totalRotation = glm::vec3(0);

    //APPLY FORCE HERE

    //Force applied once
    if(once)
    {
        once = false;
    }
    
    ApplyForce(glm::vec3(0,-9.81,0));
    for (unsigned int i = 0; i < objectIds.size(); i++)
    {
        if(own.SO_id != objects[objectIds[i]].SO_id)
        {
            if(CollisionCheckBroader(own, objects[objectIds[i]]))
            {
                if(CollisionCheckBroad(own, objects[objectIds[i]]))
                {
                    std::pair<bool, CollisionPoint> point = CollisionCheckNarrow(own, objects[objectIds[i]], 0);
                    if(point.first)
                    {
                        glm::vec3 vel = velocity;
                        velocity = glm::vec3(0);
                        glm::vec3 normal = -glm::normalize(point.second.point);
                        position += normal * point.second.dist;
                        float bounce = 0.3f;
                        float j = glm::dot(vel * -(1 + bounce), normal) / ((1 / mass) + (1 / objects[objectIds[i]].SO_rigidbody.mass));
                        ApplyImpulseForce(normal, j);
                        break;
                    }
                }
            }
        }
    }

    acceleration = totalForce / mass;
    rotAcceleration = totalRotation / mass;
    Solver(velocity, acceleration * timeStep, 1.0f);
    Solver(rotVelocity, rotAcceleration * timeStep, 1.0f);
    Solver(rotation, rotVelocity * timeStep, 1.0f);
    Solver(position, velocity * timeStep, 1.0f);

    gForce = glm::length(acceleration) / 9.81f;
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

void RigidBody::ApplyImpulseForceAtPos(glm::vec3 dir, glm::vec3 pos, float power)
{
    velocity += glm::normalize(dir) * power;
    ApplyRotationImpulseForce(glm::cross(pos, dir), power);
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


void RigidBody::CalculateCollisionShape(SpatialObject& object)
{

}