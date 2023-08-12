#pragma once
#include "../glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"
#include "../Core/Debug.hpp"
#include <iostream>

class Player
{
private:
    glm::mat4 GetRotMat();
    float speed;
public:
    glm::vec3 position;
    glm::vec3 rotation;
    Camera camera;

    Player(float speed, Camera camera, glm::vec3 position);
    ~Player();
    void UpdatePlayer();
    void Movement(int key, float delta);
};
