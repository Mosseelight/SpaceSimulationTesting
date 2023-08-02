#include <glm/glm.hpp>

class Camera
{
private:

public:

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 target;
    float fov;

    Camera(glm::vec3 position, glm::vec3 rotation, glm::vec3 target, float fov);
    glm::vec3 GetCameraDir();
    glm::vec3 GetCameraUp();
    glm::mat4 GetViewMat();
    glm::mat4 GetProjMat(int width, int height, float near, float far);
};