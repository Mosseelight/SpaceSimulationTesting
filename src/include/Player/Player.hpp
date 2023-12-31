#pragma once
#include "../glad/glad.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"
#include "../Core/Scene.hpp"
#include "../Core/Debug.hpp"
#include <iostream>

class Player
{
private:
    glm::mat4 GetRotMat();
    float speed;
public:
    glm::vec3 moveDir;
    glm::vec3 position;
    glm::vec3 rotation;
    Camera camera;
    float sensitivity = 0.2f;

    Player(float speed, Camera camera, glm::vec3 position);
    ~Player();
    void UpdatePlayer(float delta);
    void Movement(int key);
    void MouseMovement(int x, int y, bool leftP, bool rightP);
    void LaunchObject(Scene& scene, std::string modelLoc);
};
