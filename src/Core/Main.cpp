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
#include "../include/Core/Globals.hpp"
#include "../include/Core/Scene.hpp"
#include "../include/Core/Mesh.hpp"
#include "../include/Core/Shader.hpp"
#include "../include/Core/Camera.hpp"
#include "../include/Core/ResUtil.hpp"

bool DebugWindow = false;
float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastTime = 0.0f;

const std::string shaderLoc = "res/Shaders";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void Update(GLFWwindow* window);
void ImguiMenu();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void buildVerticesFlat();

Shader *shadercube;
Mesh *cube;
Camera *cam;
Scene mainScene;
unsigned int vertCount, indCount;

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
    ImGui::SetNextWindowSize(ImVec2(450,420), ImGuiCond_FirstUseEver);

    mainScene = Scene();
    shadercube = new Shader(ShaderLoc(ReadFile(shaderLoc + "/Default.vert"), ReadFile(shaderLoc + "/Default.frag")));
    Mesh mesh = CreateSphereMesh(glm::vec3(0,0,0), glm::vec3(0,0,0), 3);
    mesh.BufferGens();
    for (int i = 0; i < 1000; i++)
    {
        mainScene.AddSpaceObject(mesh, ShaderLoc(ReadFile(shaderLoc + "/Default.vert"), ReadFile(shaderLoc + "/Default.frag")));
    }
    cam = new Camera(glm::vec3(0,0,10), glm::vec3(0.0f), glm::vec3(0,0,1), 35);
    for (int i = 0; i < mainScene.SpaceObjects.size(); i++)
    {
        vertCount += mainScene.SpaceObjects[i].SO_mesh.vertexes.size();
        indCount += mainScene.SpaceObjects[i].SO_mesh.indices.size();
    }

    while(!glfwWindowShouldClose(window))
    {
        Update(window);
    }
    
    delete cam;
    delete cube;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void Update(GLFWwindow* window)
{
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    if(DebugWindow)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImguiMenu();
    }


    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.54f, 0.54f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glUseProgram(shadercube->shader);
    shadercube->setMat4("model", glm::mat4(1.0f));
    shadercube->setMat4("view", cam->GetViewMat());
    shadercube->setMat4("proj", cam->GetProjMat(SCRWIDTH, SCRHEIGHT, 0.001f, 100.0f));
    mainScene.DrawFull(1);

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


bool ShowSceneViewerMenu = false;
void ImguiMenu()
{
    
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_MenuBar;

    ImGui::Begin("SpaceTesting", nullptr, window_flags);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menus"))
        {
            ImGui::MenuItem("Scene Viewer", NULL, &ShowSceneViewerMenu);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(ShowSceneViewerMenu)
    {
        ImGui::SetNextWindowSize(ImVec2(600,420), ImGuiCond_FirstUseEver);
        ImGui::Begin("Scene Viewer");

        if (ImGui::TreeNode("Objects"))
        {
            for (int i = 0; i < mainScene.SpaceObjects.size(); i++)
            {
                if (i == 0)
                    ImGui::SetNextItemOpen(true, ImGuiCond_Once);

                if (ImGui::TreeNode((void*)(intptr_t)i, "Object %d", i))
                {
                    ImGui::DragFloat3("Cube Position", glm::value_ptr(mainScene.SpaceObjects[i].SO_mesh.position), 0.01f, -10.0f, 10.0f);
                    ImGui::DragFloat3("Cube Rotation", glm::value_ptr(mainScene.SpaceObjects[i].SO_mesh.rotation), 0.01f, 360.0f, 0.0f);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }
    ImGui::Text("App avg %.3f ms/frame (%.1f FPS)", deltaTime * 1000, round(1 / deltaTime));
    ImGui::Text("%d verts, %d indices (%d tris)", vertCount, indCount, indCount / 3);
    ImGui::Text("Amount of SpaceObjs: (%d)", mainScene.SpaceObjects.size());
    float drawCallAvg = DrawCallCount / (glfwGetTime() / deltaTime);
    ImGui::Text("DrawCall Avg: (%.1f) DC/frame, DrawCall Total (%d)", drawCallAvg, DrawCallCount);
}