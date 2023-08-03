#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../include/imgui/imgui.h"
#include "../include/backends/imgui_impl_glfw.h"
#include "../include/backends/imgui_impl_opengl3.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

//core
#include "../include/Core/Mesh.hpp"
#include "../include/Core/Shader.hpp"
#include "../include/Core/Camera.hpp"
#include "../include/Core/ResUtil.hpp"

const int SCRWIDTH = 1280;
const int SCRHEIGHT = 720;
bool DebugWindow = false;
float deltaTime = 0.0f;
float lastTime = 0.0f;

const std::string shaderLoc = "res/Shaders";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void Update(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Shader *shadercube;
Mesh *cube;
Camera *cam;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCRWIDTH, SCRHEIGHT, "SpaceSim", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     

    ImGui_ImplGlfw_InitForOpenGL(window, true);          
    ImGui_ImplOpenGL3_Init();


    shadercube = new Shader(ReadFile(shaderLoc + "/Default.vert"), ReadFile(shaderLoc + "/Default.frag"));
    Mesh mesh = CreateCubeMesh(glm::vec3(0,0,0), glm::vec3(0,0,0));
    cube = &mesh;
    cam = new Camera(glm::vec3(0,0,5), glm::vec3(0.0f), glm::vec3(0,0,1), 45);

    while(!glfwWindowShouldClose(window))
    {
        Update(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void Update(GLFWwindow* window)
{
    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    if(DebugWindow)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        
        ImGui::Begin("SpaceTesting");
        ImGui::DragFloat3("Cube Position", glm::value_ptr(cube->position), 0.01f, -5.0f, 5.0f);
    }


    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.54f, 0.54f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(shadercube->shader);
    shadercube->setMat4("model", cube->GetModelMat());
    shadercube->setMat4("view", cam->GetViewMat());
    shadercube->setMat4("proj", cam->GetProjMat(SCRWIDTH, SCRHEIGHT, 0.001f, 100.0f));
    cube->DrawMesh();

    if(DebugWindow)
    {
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
        DebugWindow = !DebugWindow;
}