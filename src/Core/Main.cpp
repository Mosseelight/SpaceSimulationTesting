#include "../include/glad/glad.h"
#include <SDL2/SDL.h>
#include "../include/imgui/imgui.h"
#include "../include/backends/imgui_impl_sdl2.h"
#include "../include/backends/imgui_impl_opengl3.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <random>

//core
#include "../include/Core/Globals.hpp"
#include "../include/Core/Scene.hpp"
#include "../include/Core/Mesh.hpp"
#include "../include/Core/Shader.hpp"
#include "../include/Player/Camera.hpp"
#include "../include/Core/ResUtil.hpp"
#include "../include/Core/Debug.hpp"

//player
#include "../include/Player/Camera.hpp"
#include "../include/Player/Player.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/3rdp/stb_image.h"

#define GetTime() SDL_GetTicks64() / 1000.0f

bool DebugWindow = false;
bool showWireFrame = false;
float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastTime = 0.0f;
int currentSCRWIDTH = 0;
int currentSCRHEIGHT = 0;
SDL_Surface windowIcon;

const std::string imageLoc = "res/Textures/";
const std::string shaderLoc = "res/Shaders/";
const std::string modelLoc = "res/Models/";

void Update(SDL_Window* window);
void ImguiMenu();
void input();

Shader shader;
std::unique_ptr<Player> player;
Scene mainScene;
unsigned int vertCount, indCount;

int run = 1;
int main()
{

    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    SDL_Window* window = SDL_CreateWindow("SpaceSim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    windowIcon.pixels = stbi_load((imageLoc + "/IconSpace.png").c_str(), &windowIcon.w, &windowIcon.h, 0, 4);
    //SDL_SetWindowIcon(window, &windowIcon);
    stbi_image_free(windowIcon.pixels);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
    ImGui::SetNextWindowSize(ImVec2(450,420), ImGuiCond_FirstUseEver);

    mainScene.AddSpaceObject(LoadModel(glm::vec3(0,-1,0), glm::vec3(0), modelLoc + "Bunny.obj"));
    mainScene.AddSpaceObject(LoadModel(glm::vec3(3,0,0), glm::vec3(0), modelLoc + "Torus.obj"));
    mainScene.AddSpaceObject(CreateSphereMesh(glm::vec3(-3,0,0), glm::vec3(0,0,0), 2));

    player.reset(new Player(20.0f, Camera(glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0,0,-1), 35), glm::vec3(0,0,10)));
    shader.CompileShader(ShaderLoc(ReadFile(shaderLoc + "Default.vert"), ReadFile(shaderLoc + "Default.frag")));
    for (int i = 0; i < mainScene.SpaceObjects.size(); i++)
    {
        vertCount += mainScene.SpaceObjects[i].SO_mesh.vertexes.size();
        indCount += mainScene.SpaceObjects[i].SO_mesh.indices.size();
    }

    while(run)
    {
        Update(window);
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    CreateFileLog();
    return 0;
}

void Update(SDL_Window* window)
{
    SDL_GetWindowSize(window, &currentSCRWIDTH, &currentSCRHEIGHT);

    currentTime = GetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    drawCallAvg = DrawCallCount / (GetTime() / deltaTime);

    player->UpdatePlayer();

    if(DebugWindow)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImguiMenu();
    }

    glClearColor(0.0f, 0.54f, 0.54f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    if(showWireFrame)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glUseProgram(shader.shader);
    shader.setMat4("proj", player->camera.GetProjMat(currentSCRWIDTH, currentSCRHEIGHT, 0.001f, 100000.0f));
    shader.setMat4("view", player->camera.GetViewMat());
    mainScene.DrawSingle(&shader);

    if(DebugWindow)
    {
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    input();
    SDL_GL_SwapWindow(window);
}

void input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            run = 0;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_BACKQUOTE:
                DebugWindow = !DebugWindow;
                break;
            }
            break;
        }
    }
    /*
    if(key == GLFW_KEY_W && action == GLFW_PRESS)
        player->Movement(key, deltaTime);
    if(key == GLFW_KEY_S && action == GLFW_PRESS)
        player->Movement(key, deltaTime);
    if(key == GLFW_KEY_A && action == GLFW_PRESS)
        player->Movement(key, deltaTime);
    if(key == GLFW_KEY_D && action == GLFW_PRESS)
        player->Movement(key, deltaTime);
    if(key == GLFW_KEY_W && action == GLFW_REPEAT)
        player->Movement(key, deltaTime);
    if(key == GLFW_KEY_S && action == GLFW_REPEAT)
        player->Movement(key, deltaTime);
    if(key == GLFW_KEY_A && action == GLFW_REPEAT)
        player->Movement(key, deltaTime);
    if(key == GLFW_KEY_D && action == GLFW_REPEAT)
        player->Movement(key, deltaTime);*/
}


bool ShowSceneViewerMenu = false;
bool ShowObjectViewerMenu = false;
bool ShowConsoleViewerMenu = false;
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
    ImGui::Text("Ram Usage: %.2fmb", GetRamUsage() / 1024);
    ImGui::Text("Time Open %.1f minutes", (GetTime() / 60));

    ImGui::Spacing();
    ImGui::Checkbox("Wire Frame", &showWireFrame);
    ImGui::SliderFloat3("Player Position", glm::value_ptr(player->position), -50.0f, 50.0f);
    ImGui::SliderFloat3("Player Rotation", glm::value_ptr(player->rotation), 360.0f, 0.0f);
    ImGui::SliderFloat("Cam Fov", &player->camera.fov, 179.9f, 0.01f);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menus"))
        {
            ImGui::MenuItem("Scene Viewer", NULL, &ShowSceneViewerMenu);
            ImGui::MenuItem("Object Viewer", NULL, &ShowObjectViewerMenu);
            ImGui::MenuItem("Console Viewer", NULL, &ShowConsoleViewerMenu);
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
                    ImGui::DragFloat3("Object Position", glm::value_ptr(mainScene.SpaceObjects[i].SO_mesh.position), 0.01f, -100000.0f, 100000.0f);
                    ImGui::DragFloat3("Object Rotation", glm::value_ptr(mainScene.SpaceObjects[i].SO_mesh.rotation), 0.1f, 360.0f, 360.0f);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }

    if(ShowObjectViewerMenu)
    {
        static bool objectSelected = false;
        static int counter;
        static glm::vec3 selposition;
        static glm::vec3 selrotation;
        static int IcoSphereSub = 0;
        static char input[128] = "mesh.obj";
        MeshType type;
        ImGui::SetNextWindowSize(ImVec2(600,420), ImGuiCond_FirstUseEver);
        ImGui::Begin("Object Viewer");

        //select the object you want gives properties 
        //or create a object and that selects by default
        ImGui::Text("Select Object Properties");
        ImGui::Spacing();
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
        { 
            if(counter > MeshType::First)
                counter--;
        }
        ImGui::SameLine(0.0f, 1.0f);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) 
        { 
            if(counter < MeshType::Last)
                counter++; 
        }
        ImGui::SameLine();
        ImGui::Text(GetMeshTypeName(static_cast<MeshType>(counter)).c_str());
        ImGui::InputFloat3("Object Position", glm::value_ptr(selposition));
        ImGui::InputFloat3("Object Rotation", glm::value_ptr(selrotation));
        if(counter == MeshType::IcoSphereMesh)
            ImGui::SliderInt("IcoSphere Subdivison level", &IcoSphereSub, 0, 10);
        if(counter == MeshType::FileMesh)
        {
            ImGui::Text(modelLoc.c_str());
            ImGui::SameLine();
            ImGui::InputText("Mesh File Name", input, IM_ARRAYSIZE(input));
        }

        if(ImGui::Button("Add Object"))
        {
            Mesh selmesh = Mesh();
            switch (static_cast<MeshType>(counter))
            {
            case CubeMesh:
                selmesh = CreateCubeMesh(selposition, selrotation);
                break;
            case IcoSphereMesh:
                selmesh = CreateSphereMesh(selposition, selrotation, IcoSphereSub);
                break;
            case TriangleMesh:
                selmesh = Create2DTriangle(selposition, selrotation);
                break;
            case FileMesh:
                selmesh = LoadModel(selposition, selrotation, modelLoc + input);
            }
            objectSelected = true;
            vertCount += selmesh.vertexes.size() * 3;
            indCount += selmesh.indices.size();
            mainScene.AddSpaceObject(selmesh);
        }

        ImGui::End();
    }

    if(ShowConsoleViewerMenu)
    {
        ImGui::SetNextWindowSize(ImVec2(600,420), ImGuiCond_FirstUseEver);
        ImGui::Begin("Console Viewer");
        ImGui::End();
    }
}