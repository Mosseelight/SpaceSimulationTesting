#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Globals.hpp"

struct SpatialObject;
struct Vertex;
class Mesh;

class BoundingBox
{
private:
public:
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 minOri;
    glm::vec3 maxOri;
    bool foundBox;

    BoundingBox();

    void ConstructBoundingBox(Mesh& mesh);
    void ConstructOriBoundingBox(Mesh& mesh);
};

class RigidBody
{
private:
    glm::vec3 oldPos;
    
    glm::vec3 forward;
    glm::vec3 backward;
    glm::vec3 right;
    glm::vec3 left;
    glm::vec3 up;
    glm::vec3 down;

    glm::mat3 inertiaT;

public:

    BoundingBox boundbox;
    BoundingBox oriBoundBox;
    std::vector<Vertex> collisionShape;
    bool isStatic;
    bool isAwake;

    float mass;
    float volume;
    float density;
    float friction;
    glm::vec3 massCenter;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    glm::vec3 rotation;
    glm::vec3 rotVelocity;
    glm::vec3 rotAcceleration;

    glm::vec3 totalForce;
    glm::vec3 totalRotation;

    float gForce;

    RigidBody();
    ~RigidBody();
    void Step(float timeStep, float deltaTime,  std::vector<unsigned int>& objectIds, std::vector<SpatialObject>& objects, SpatialObject& own);
    void ApplyForce(glm::vec3 force);
    void ApplyForceAtPos(glm::vec3 force, glm::vec3 pos);
    void ApplyImpulseForce(glm::vec3 dir, float power);
    void ApplyImpulseForceAtPos(glm::vec3 dir, glm::vec3 pos, float power);
    void ApplyDragForce(float airDensity, float area);
    void ApplyLiftForce(float airDesnity, float area);

    void ApplyRotationForce(glm::vec3 force);
    void ApplyRotationImpulseForce(glm::vec3 dir, float power);
    glm::vec3 GetLocalDir(glm::vec3 dir);

    void CalculateCollisionShape(SpatialObject& object);
    void CalculateInertiaTensor(SpatialObject& object);
    void CalculateInertiaTensorCube(SpatialObject& object);
    void ApplyImpulseForceAtPos2(glm::vec3 dir, glm::vec3 pos, float power);
    void CheckSleep();
};
