#include "../../include/Core/Physics/RigidBody.hpp"
#include "../../include/Core/Physics/Collision.hpp"
#include "../../include/Core/Scene.hpp"
#include "../../include/Core/Math.hpp"

void Solver(glm::vec3& out, glm::vec3 in, float step);
void CollisionSolve(SpatialObject& own, SpatialObject& other, CollisionPoint point);

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
        glm::vec3 vertexPos = TransformVec4(glm::vec4(mesh.vertexes[i].position, 1.0f), mesh.rotMatrix);

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
    
    min = mintmp + mesh.position * mesh.scale;
    max = maxtmp + mesh.position * mesh.scale;
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
    mass = 1000.0f;
    density = 50.0f;
    friction = 0.99f;
    position = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    rotVelocity = glm::vec3(0.0f);
    rotAcceleration = glm::vec3(0.0f);
    totalForce = glm::vec3(0.0f);
    totalRotation = glm::vec3(0.0f);
    isStatic = false;
    isAwake = true;
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
    oldPos = position;

    //APPLY FORCE HERE

    //Force applied once
    if(once)
    {
        once = false;
    }
    
    ApplyForce(glm::vec3(0,-9.81,0) * mass);
    //ApplyLiftForce(AirDensity, 28.0f);
    //ApplyDragForce(AirDensity, 1.0f);
    //pplyImpulseForceAtPos2(glm::vec3(0,0,1), glm::vec3(0,10,0), 1.0f);
    for (unsigned int i = 0; i < objectIds.size(); i++)
    {
        if(own.SO_id != objects[objectIds[i]].SO_id)
        {
            if(CollisionCheckBroad(own, objects[objectIds[i]]))
            {
                if(own.SO_id != objects[objectIds[i]].SO_id)
                {
                    std::pair<bool, CollisionPoint> point = CollisionCheckNarrowGjk(own, objects[objectIds[i]]);
                    if(point.first)
                    {
                        DrawDebugCube(point.second.point, 0.1f, glm::vec3(0,0,255));
                        CollisionSolve(own, objects[objectIds[i]], point.second);
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

    //calculate local directions
    forward = TransformVec4(glm::vec4(0,0,1,1), own.SO_mesh.rotMatrix);
    right = TransformVec4(glm::vec4(01,0,0,1), own.SO_mesh.rotMatrix);
    backward = -forward;
    left = -right;
    up = glm::cross(forward, right);
    down = -up;

    //CalculateInertiaTensorCube(own);
    //for (unsigned int i = 0; i < 3; i++)
    //{
    //    std::cout << inertiaT[i][0] << " " << inertiaT[i][1] << " " << inertiaT[i][2] << std::endl;
    //}
    CalculateInertiaTensor(own);
    //for (unsigned int i = 0; i < 3; i++)
    //{
    //    std::cout << inertiaT[i][0] << " " << inertiaT[i][1] << " " << inertiaT[i][2] << std::endl;
    //}

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
    velocity = dir * power;
}

void RigidBody::ApplyImpulseForceAtPos(glm::vec3 dir, glm::vec3 pos, float power)
{
    velocity = dir * power;
    ApplyRotationImpulseForce(glm::cross(pos, dir), power);
}

void RigidBody::ApplyImpulseForceAtPos2(glm::vec3 dir, glm::vec3 pos, float power)
{
    //velocity = dir * power;
    glm::vec3 rot = inertiaT * dir;
    ApplyRotationImpulseForce(rot / 10.0f, power);
}

void RigidBody::ApplyDragForce(float airDensity, float area)
{
    totalForce += 0.5f * airDensity * area * 0.0175f * -velocity;
}

void RigidBody::ApplyLiftForce(float airDensity, float area)
{
    totalForce += up * (0.5f * airDensity * glm::length(velocity) * glm::length(velocity) * area * 1.9f);
}

void RigidBody::ApplyRotationForce(glm::vec3 force)
{
    totalRotation += force;
}

void RigidBody::ApplyRotationImpulseForce(glm::vec3 dir, float power)
{
    rotVelocity = dir * power;
}

//dont use
glm::vec3 RigidBody::GetLocalDir(glm::vec3 dir)
{
    glm::mat4 rotMat = glm::mat4(1.0f);
    rotMat = glm::rotate(rotMat, rotation.x * 3.14159265358979323846f/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.y * 3.14159265358979323846f/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    rotMat = glm::rotate(rotMat, rotation.z * 3.14159265358979323846f/180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    return TransformVec4(glm::vec4(dir, 1.0f), rotMat);
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

void RigidBody::CheckSleep()
{
        if(glm::distance(position, oldPos) > 0.0f)
            isAwake = true;
        else
            isAwake = false;
}

void RigidBody::CalculateCollisionShape(SpatialObject& object)
{

}

// Source: https://github.com/blackedout01/simkn/blob/main/simkn.h

float ITTet3InertiaMoment(glm::mat3 P, unsigned int I) 
{
    float a = P[0][I];
    float b = P[1][I];
    float c = P[2][I];
    return (a * a) + b * c + (b * b) + a * c + (c * c) + a * b;
}


float ITTet3IntertiaProduct(glm::mat3 P, unsigned int I, unsigned int J) 
{
    float a = P[0][I];
    float b = P[0][J];
    float c = P[1][I];
    float d = P[1][J];
    float e = P[2][I];
    float f = P[2][J];
    return 2.0f * a * b + c * f + e * d + 2.0f * c * d + a * f + e * b + 2.0f * e * f + a * d + c * b;
}

void RigidBody::CalculateInertiaTensorCube(SpatialObject& object) 
{
    float XX = boundbox.max.x * boundbox.max.x;
    float YY = boundbox.max.y * boundbox.max.y;
    float ZZ = boundbox.max.z * boundbox.max.z;
    float Mass = density * boundbox.max.x * boundbox.max.y * boundbox.max.z;
    inertiaT[0][0] = Mass * (YY + ZZ) / 12.0f;
    inertiaT[0][1] = Mass * (XX + ZZ) / 12.0f;
    inertiaT[0][2] = Mass * (XX + YY) / 12.0f;
}

void RigidBody::CalculateInertiaTensor(SpatialObject& object)
{
    float Mass = 0.0f;
    float Volume = 0.0f;
    glm::vec3 MassCenter = glm::vec3(0.0f);
    float Ia = 0.0f, Ib = 0.0f, Ic = 0.0f, Iap = 0.0f, Ibp = 0.0f, Icp = 0.0f;
    glm::mat3 P = glm::mat3(0.0f);
    for(unsigned int I = 0; I < object.SO_mesh.vertexes.size(); I += 3) 
    {
        P[0] = TransformVec4(glm::vec4(object.SO_mesh.vertexes[I].position, 1.0f), object.SO_mesh.modelMatrix);
        P[1] = TransformVec4(glm::vec4(object.SO_mesh.vertexes[I + 1].position, 1.0f), object.SO_mesh.modelMatrix);
        P[2] = TransformVec4(glm::vec4(object.SO_mesh.vertexes[I + 2].position, 1.0f), object.SO_mesh.modelMatrix);
        P = glm::transpose(P);

        float DetJ = glm::dot(P[0], glm::cross(P[1], P[2]));
        //divided by 8 seems to fix a problem where the mass is 8 times what it should be
        float TetVolume = (DetJ / 6.0f) / 8.0f;
        float TetMass = density * TetVolume;
        
        glm::vec3 TetMassCenter = glm::vec3(0.0f);
        TetMassCenter += P[0];
        TetMassCenter += P[1];
        TetMassCenter += P[2];
        TetMassCenter /= 4.0f;
        
        float V100 = ITTet3InertiaMoment(P, 0);
        float V010 = ITTet3InertiaMoment(P, 1);
        float V001 = ITTet3InertiaMoment(P, 2);
        
        Ia += DetJ * (V010 + V001);
        Ib += DetJ * (V100 + V001);
        Ic += DetJ * (V100 + V010);
        Iap += DetJ * ITTet3IntertiaProduct(P, 1, 2);
        Ibp += DetJ * ITTet3IntertiaProduct(P, 0, 1);
        Icp += DetJ * ITTet3IntertiaProduct(P, 0, 2);

        TetMassCenter *= TetMass;
        MassCenter += TetMassCenter;
        Mass += TetMass;
        Volume += TetVolume;
    }
    
    MassCenter /= Mass;
    Ia = density * Ia / 60.0f - Mass*((MassCenter[1] * MassCenter[1]) + (MassCenter[2] * MassCenter[2]));
    Ib = density * Ib / 60.0f - Mass*((MassCenter[0] * MassCenter[0]) + (MassCenter[2] * MassCenter[2]));
    Ic = density * Ic / 60.0f - Mass*((MassCenter[0] * MassCenter[0]) + (MassCenter[1] * MassCenter[1]));
    Iap = density * Iap / 120.0f - Mass*(MassCenter[1] * MassCenter[2]);
    Ibp = density * Ibp / 120.0f - Mass*(MassCenter[0] * MassCenter[1]);
    Icp = density * Icp / 120.0f - Mass*(MassCenter[0] * MassCenter[2]);
    
    inertiaT[0][0] = Ia;
    inertiaT[1][0] = Ib;
    inertiaT[2][0] = Ic;
    inertiaT[0][1] = inertiaT[0][2] = -Ibp;
    inertiaT[0][2] = inertiaT[1][2] = -Icp;
    inertiaT[1][1] = inertiaT[2][2] = -Iap;
    
    massCenter = MassCenter; 
    mass = Mass;
    volume = Volume;
}

void CollisionSolve(SpatialObject& own, SpatialObject& other, CollisionPoint point)
{
    glm::vec3 normal = -glm::normalize(point.normal);
    if(normal.x - roundf(normal.x) < 0.001)
        normal.x = roundf(normal.x);
    if(normal.y - roundf(normal.y) < 0.001)
        normal.y = roundf(normal.y);
    if(normal.z - roundf(normal.z) < 0.001)
        normal.z = roundf(normal.z);
    if(other.SO_rigidbody.isStatic)
    {
        own.SO_rigidbody.position += normal * point.dist * 0.5f;
        float bounce = 0.6f;
        float j = glm::dot(own.SO_rigidbody.velocity * -(1 + bounce), normal) / glm::dot(normal * (1 / own.SO_rigidbody.mass), normal);
        own.SO_rigidbody.ApplyImpulseForce(own.SO_rigidbody.velocity + normal * (j / own.SO_rigidbody.mass), 1.0f);
        //own.SO_rigidbody.velocity += own.SO_rigidbody.velocity * -own.SO_rigidbody.friction;
        //ApplyRotationImpulseForce(rotVelocity + j * glm::cross(point.second.point, normal), 1.0f);
        return;
    }
    else
    {
        glm::vec3 sizeOther = other.SO_rigidbody.oriBoundBox.max * 0.5f;
        glm::vec3 size = own.SO_rigidbody.oriBoundBox.max * 0.5f;
        if (own.SO_rigidbody.position.y > other.SO_rigidbody.position.y + std::min(sizeOther.x, std::min(sizeOther.y, sizeOther.z))) 
        {
        	own.SO_rigidbody.position += normal * point.dist * 0.8f;
            other.SO_rigidbody.position += normal * point.dist * -0.2f;
        } 
        else if (other.SO_rigidbody.position.y > own.SO_rigidbody.position.y + std::min(size.x, std::min(size.y, size.z))) 
        {
        	own.SO_rigidbody.position += normal * point.dist * 0.2f;
            other.SO_rigidbody.position += normal * point.dist * -0.8f;
        } 
        else 
        {
            own.SO_rigidbody.position += normal * point.dist * 0.5f;
            other.SO_rigidbody.position += normal * point.dist * -0.5f;
        }
        float bounce = 0.6f;
        float j = glm::dot(own.SO_rigidbody.velocity * -(1 + bounce), normal) / glm::dot(normal * (1 / own.SO_rigidbody.mass + 1 / other.SO_rigidbody.mass), normal);
        own.SO_rigidbody.ApplyImpulseForce(own.SO_rigidbody.velocity + normal * (j / own.SO_rigidbody.mass), 1.0f);
        //ApplyRotationImpulseForce(rotVelocity + j * glm::inverse(inertiaT) * glm::cross(normal, normal), 1.0f);
        other.SO_rigidbody.ApplyImpulseForce(other.SO_rigidbody.velocity + -normal * (j / other.SO_rigidbody.mass), 1.0f);
        //objects[i].SO_rigidbody.ApplyRotationImpulseForce(objects[i].SO_rigidbody.rotVelocity
        // + j * glm::inverse(objects[i].SO_rigidbody.inertiaT) * glm::cross(-normal, -normal), 1.0f);
        //own.SO_rigidbody.velocity += own.SO_rigidbody.velocity * -own.SO_rigidbody.friction;
        //other.SO_rigidbody.velocity += other.SO_rigidbody.velocity * -other.SO_rigidbody.friction;
        return;
    }
}
