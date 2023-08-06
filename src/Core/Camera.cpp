#include "../include/Core/Camera.hpp"

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
    //target.x = cos(glm::radians(rotation.z) * cos(glm::radians(rotation.x)));
    //target.y = sin(glm::radians(rotation.x));
    //target.x = sin(glm::radians(rotation.z) * cos(glm::radians(rotation.x)));
    return glm::normalize(position - target);
}

glm::vec3 Camera::GetCameraUp()
{
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, GetCameraDir()));
    return glm::cross(GetCameraDir(), cameraRight);
}

glm::mat4 Camera::GetViewMat()
{
    return glm::lookAt(position, GetCameraDir(), GetCameraUp());
}

glm::mat4 Camera::GetProjMat(int width, int height, float near, float far)
{
    return glm::perspectiveFov(glm::radians(fov), (float)width, (float)height, near, far);
}