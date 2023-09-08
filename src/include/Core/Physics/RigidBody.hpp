#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Globals.hpp"
#include "../Mesh.hpp"

class BoundingBox
{
private:
public:
    glm::vec3 min;
    glm::vec3 max;

    BoundingBox();
    ~BoundingBox();

    void ConstructBoundingBox(Mesh& mesh);
};

class RigidBody
{
private:
    
public:

    BoundingBox boundbox;

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
    void Step(float timeStep);
    void ApplyForce(glm::vec3 force);
    void ApplyForceAtPos(glm::vec3 force, glm::vec3 pos);
    void ApplyImpulseForce(glm::vec3 dir, float power);
    void ApplyDragForce(float airDensity, float area);

    void ApplyRotationForce(glm::vec3 force);
    void ApplyRotationImpulseForce(glm::vec3 dir, float power);
    glm::vec3 GetLocalDir(glm::vec3 dir);
};
