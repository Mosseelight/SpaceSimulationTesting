#pragma once
#include <iostream>
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

    BoundingBox();

    void ConstructBoundingBox(Mesh& mesh);
};

class RigidBody
{
private:
    
public:

    BoundingBox boundbox;
    std::vector<Vertex> collisionShape;
    bool isStatic;

    float mass;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    glm::vec3 rotation;
    glm::vec3 rotVelocity;
    glm::vec3 rotAcceleration;

    glm::vec3 totalForce;
    glm::vec3 totalRotation;

    RigidBody();
    ~RigidBody();
    void Step(float timeStep, std::vector<SpatialObject>& objects, SpatialObject& own);
    void ApplyForce(glm::vec3 force);
    void ApplyForceAtPos(glm::vec3 force, glm::vec3 pos);
    void ApplyImpulseForce(glm::vec3 dir, float power);
    void ApplyDragForce(float airDensity, float area);

    void ApplyRotationForce(glm::vec3 force);
    void ApplyRotationImpulseForce(glm::vec3 dir, float power);
    glm::vec3 GetLocalDir(glm::vec3 dir);

    void CalculateCollisionShape(SpatialObject& object);
};
