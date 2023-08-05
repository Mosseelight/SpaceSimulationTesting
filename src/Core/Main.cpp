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
bool showWireFrame = false;
float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastTime = 0.0f;
int currentSCRWIDTH = 0;
int currentSCRHEIGHT = 0;

const std::string shaderLoc = "res/Shaders";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void Update(GLFWwindow* window);
void ImguiMenu();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void buildVerticesFlat();

Shader shader;
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
    for (int i = 0; i < 10; i++)
    {
        //mainScene.AddSpaceObject(CreateSphereMesh(glm::vec3(0,0,0), glm::vec3(0,0,0), 3));
        mainScene.AddSpaceObject(CreateCubeMesh(glm::vec3(0,0,0), glm::vec3(0,0,0)));
        mainScene.SpaceObjects[i].SO_mesh.BufferGens();
    }
    cam = new Camera(glm::vec3(0,0,10), glm::vec3(0.0f), glm::vec3(0,0,0), 35);
    shader = Shader();
    shader.CompileShader(ShaderLoc(ReadFile(shaderLoc + "/Default.vert"), ReadFile(shaderLoc + "/Default.frag")));
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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void Update(GLFWwindow* window)
{
    glfwGetWindowSize(window, &currentSCRWIDTH, &currentSCRHEIGHT);

    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    drawCallAvg = DrawCallCount / (glfwGetTime() / deltaTime);

    if(DebugWindow)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImguiMenu();
    }



    glClearColor(0.0f, 0.54f, 0.54f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    if(showWireFrame)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    cam->position.x = sin(glfwGetTime()) * 10;
    cam->position.z = cos(glfwGetTime()) * 10;

    glUseProgram(shader.shader);
    shader.setMat4("proj", cam->GetProjMat(currentSCRWIDTH, currentSCRHEIGHT, 0.001f, 100.0f));
    shader.setMat4("view", cam->GetViewMat());

    for (unsigned int i = 0; i < mainScene.SpaceObjects.size(); i++)
    {
        mainScene.SpaceObjects[i].SO_mesh.position.y = sin(glfwGetTime());

        shader.setMat4("model", mainScene.SpaceObjects[i].SO_mesh.GetModelMat());
        mainScene.SpaceObjects[i].SO_mesh.DrawMesh();
    }

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

    ImGui::Text("App avg %.3f ms/frame (%.1f FPS)", deltaTime * 1000, round(1 / deltaTime));
    ImGui::Text("%d verts, %d indices (%d tris)", vertCount, indCount, indCount / 3);
    ImGui::Text("Amount of SpaceObjs: (%d)", mainScene.SpaceObjects.size());
    ImGui::Text("DrawCall Avg: (%.1f) DC/frame, DrawCall Total (%d)", drawCallAvg, DrawCallCount);
    ImGui::Text("Ram Usage: %dmb", GetRamUsage() / 1024);

    ImGui::Spacing();
    ImGui::Checkbox("Wire Frame", &showWireFrame);
    ImGui::DragFloat3("Cam Position", glm::value_ptr(cam->position), 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat3("Cam Rotation", glm::value_ptr(cam->rotation), 0.01f, 360.0f, 0.0f);

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
}