#include "../include/Player/Camera.hpp"

Camera::Camera()
{

}

Camera::Camera(glm::vec3 position, glm::vec3 rotation, glm::vec3 target, float fov)
{
    this->position = position;
    this->rotation = rotation;
    this->target = target;
    this->fov = fov;
}

glm::vec3 Camera::GetCameraDir()
{
    target.x = -sinf(rotation.x*(M_PI/180)) * cosf((rotation.y)*(M_PI/180));
    target.y = -sinf((rotation.y)*(M_PI/180));
    target.z = cosf((rotation.x)*(M_PI/180)) * cosf((rotation.y)*(M_PI/180));
    return glm::normalize(target);
}

glm::vec3 Camera::GetCameraUp()
{
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, GetCameraDir()));
    return glm::cross(GetCameraDir(), cameraRight);
}

glm::mat4 Camera::GetViewMat()
{
    return glm::lookAt(position, position + GetCameraDir(), GetCameraUp());
}

glm::mat4 Camera::GetProjMat(int width, int height, float near, float far)
{
    return glm::perspective(glm::radians(fov), (float)width / (float)height, near, far);
}