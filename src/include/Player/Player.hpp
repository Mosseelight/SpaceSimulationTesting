#pragma once
#include <glm/glm.hpp>
#include "../Core/Camera.hpp"

class Player
{
private:
    
public:
    glm::vec3 position;
    glm::vec3 rotation;
    Camera camera;

    Player();
    ~Player();
    void Movement();
};
