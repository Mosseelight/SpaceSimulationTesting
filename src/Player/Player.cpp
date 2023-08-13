#include "../include/Player/Player.hpp"

Player::Player(float speed, Camera camera, glm::vec3 position)
{
    this->speed = speed;
    this->camera = camera;
    this->position = position;
}

Player::~Player()
{
    DebugLog("Player got deleted");
}  

void Player::UpdatePlayer()
{
    camera.position = position + glm::vec3(0,0,0);
}

glm::mat4 Player::GetRotMat()
{
    glm::mat4 mat;
    mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f,0.0f,0.0f));
    mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f,1.0f,0.0f));
    mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f,0.0f,1.0f));
    return mat;
}

void Player::Movement(int key, float delta)
{
    glm::vec3 moveDir;
    glm::mat4 rotMat = GetRotMat();
    glm::vec3 forward(0.0f,0.0f,-1.0f);
    glm::vec3 backward(0.0f,0.0f,1.0f);
    glm::vec3 right(1.0f,0.0f,0.0f);
    glm::vec3 left(-1.0f,0.0f,0.0f);
    glm::vec3 up(0.0f,1.0f,0.0f);
    glm::vec3 down(0.0f,-1.0f,0.0f);
    //forward = glm::vec3(glm::vec4(forward, 1.0f) * rotMat);
    //backward = glm::vec3(glm::vec4(backward, 1.0f) * rotMat);
    //left = glm::vec3(glm::vec4(left, 1.0f) * rotMat);
    //right = glm::vec3(glm::vec4(right, 1.0f) * rotMat);
    switch (key)
    {
    case SDLK_w:
        moveDir = forward;
        break;
    case SDLK_s:
        moveDir = backward;
        break;
    case SDLK_a:
        moveDir = left;
        break;
    case SDLK_d:
        moveDir = right;
        break;
    case SDLK_SPACE:
        moveDir = up;
        break;
    case SDLK_LSHIFT:
        moveDir = down;
        break;
    }
    moveDir = glm::normalize(moveDir);
    position += moveDir * speed * delta;
    
}