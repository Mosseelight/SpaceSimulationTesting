#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>

//core
#include "../include/Core/Mesh.hpp"
#include "../include/Core/Shader.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

char *VertexShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPosition;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPosition, 1.0);\n"
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
        -4.0f, -4.0f,  4.0f,
        4.0f, -4.0f,  4.0f,
        4.0f,  4.0f,  4.0f,
        -4.0f,  4.0f,  4.0f,
        -4.0f, -4.0f, -4.0f,
        4.0f, -4.0f, -4.0f,
        4.0f,  4.0f, -4.0f,
        -4.0f,  4.0f, -4.0f
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
    Mesh cube = Mesh(vertices, indices);

    while(!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.54f, 0.54f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shadercube.shader);
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