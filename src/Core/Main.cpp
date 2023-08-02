#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>

//core
#include "../include/Core/Mesh.hpp"
#include "../include/Core/Shader.hpp"
#include "../include/Core/Camera.hpp"

const int SCRWIDTH = 1920;
const int SCRHEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

char *VertexShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPosition;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"void main()\n"
"{\n"
"    gl_Position = proj * view * model * vec4(aPosition, 1.0);\n"
"}\0";

char *FragmentShader = "#version 330 core\n"
"out vec4 out_color;\n"
"void main()\n"
"{\n"
"    out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";

int main()
{
    float vertices[] =
    {
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
    };

    unsigned int indices[] =
    {
        0, 1, 2,
	    2, 3, 0,
        1, 5, 6,
        6, 2, 1,
        7, 6, 5,
        5, 4, 7,
        4, 0, 3,
        3, 7, 4,
        4, 5, 1,
        1, 0, 4,
        3, 2, 6,
        6, 7, 3
    };

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "SpaceSim", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shadercube = Shader(VertexShader, FragmentShader);
    Mesh cube = Mesh(vertices, indices, sizeof(vertices) / sizeof(vertices[0]), sizeof(indices) / sizeof(indices[0]));
    Camera cam = Camera(glm::vec3(0,0,5), glm::vec3(0.0f), glm::vec3(0,0,1), 45);

    while(!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.54f, 0.54f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shadercube.shader);
        shadercube.setMat4("model", glm::mat4(1.0f));
        shadercube.setMat4("view", cam.GetViewMat());
        shadercube.setMat4("proj", cam.GetProjMat(SCRWIDTH, SCRHEIGHT, 0.001f, 100.0f));
        cube.DrawMesh();

        glfwSwapBuffers(window);
        glfwPollEvents();   
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  