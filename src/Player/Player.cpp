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
    camera.position = position;
    camera.rotation = rotation;
}

glm::mat4 Player::GetRotMat()
{
    glm::mat4 mat = glm::mat4(1.0f);
    mat = glm::rotate(mat, rotation.x * M_PIf/180.0f, glm::vec3(1.0f,0.0f,0.0f));
    mat = glm::rotate(mat, rotation.y * M_PIf/180.0f, glm::vec3(0.0f,1.0f,0.0f));
    mat = glm::rotate(mat, rotation.z * M_PIf/180.0f, glm::vec3(0.0f,0.0f,1.0f));
    return mat;
}

void Player::Movement(int key, float delta)
{
    glm::vec3 moveDir;
    glm::mat4 rotMat = GetRotMat();
    glm::vec3 up(0.0f,0.1f,0.0f);
    glm::vec3 down(0.0f,-0.1f,0.0f);
    switch (key)
    {
    case SDLK_w:
        moveDir = camera.GetCameraDir();
        break;
    case SDLK_s:
        moveDir = -camera.GetCameraDir();
        break;
    case SDLK_a:
        moveDir = -glm::cross(camera.GetCameraDir(), camera.GetCameraUp());
        break;
    case SDLK_d:
        moveDir = glm::cross(camera.GetCameraDir(), camera.GetCameraUp());
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

void Player::MouseMovement(int x, int y, bool leftP, bool rightP)
{
    rotation.x += x * sensitivity;
    rotation.y += y * sensitivity;
}