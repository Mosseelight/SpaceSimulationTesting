#include "../include/glad/glad.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/imgui/imgui.h"
#include "../include/imgui/implot.h"
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
#include "../include/Core/ResUtil.hpp"
#include "../include/Core/Debug.hpp"
#include "../include/Core/Texture.hpp"

//physics
#include "../include/Core/Physics/SpatialPhysics.hpp"
#include "../include/Core/Physics/Chunks.hpp"

//player
#include "../include/Player/Camera.hpp"
#include "../include/Player/Player.hpp"

#define GetTime() SDL_GetTicks64() / 1000.0f

bool DebugWindow = false;
bool showWireFrame = false;
float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastTime = 0.0f;
int currentSCRWIDTH = 0;
int currentSCRHEIGHT = 0;
SDL_Surface *windowIcon;
std::string platform;

const std::string imageLoc = "res/Textures/";
const std::string shaderLoc = "res/Shaders/";
const std::string modelLoc = "res/Models/";
const std::string logLoc = "res/Logs/";
const std::string sceneLoc = "res/Scenes/";

void UpdateLogic(SDL_Window* window);
void Render(SDL_Window* window);
void ImguiMenu();
void input();

Shader shader;
Texture texture;
std::unique_ptr<Player> player;
Scene mainScene;
unsigned int vertCount, indCount;

int run = 1;
int main()
{
    
    //Sdl creation
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    
    SDL_Window* window = SDL_CreateWindow("SpaceSim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    windowIcon = IMG_Load((imageLoc + "/IconSpace.png").c_str());
    SDL_SetWindowIcon(window, windowIcon);

    platform = SDL_GetPlatform();

    //ImGui Creation
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
    ImGui::SetNextWindowSize(ImVec2(450,420), ImGuiCond_FirstUseEver);


    //scene initilzation
    texture.LoadTexture(imageLoc + "IconSpace.png");

    unsigned int count = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int g = 0; g < 3; g++)
        {
            mainScene.AddSpatialObject(LoadModel(glm::vec3(i * 100, -0.7f, g * 100), glm::vec3(0,0,0), modelLoc + "Floor.obj"));
            mainScene.SpatialObjects[count].SO_rigidbody.isStatic = true;
            count++;
        }
    }

    //mainScene.AddSpatialObject(LoadModel(glm::vec3(0,0,0), glm::vec3(0), modelLoc + "Bunny.obj"));
    //mainScene.AddSpatialObject(LoadModel(glm::vec3(3,5,0), glm::vec3(0), modelLoc + "Monkey.obj"));
    //mainScene.AddSpatialObject(LoadModel(glm::vec3(0,5,-4), glm::vec3(0), modelLoc + "Teapot.obj"));
    
    for (int i = -48; i < 48 + (100 * 2); i += 3)
    {
        for (int g = -48; g < 48 + (100 * 2); g += 3)
        {
            mainScene.AddSpatialObject(CreateCubeMesh(glm::vec3(i,3,g), glm::vec3(0,0,0)));
        }
    }

    for (unsigned int i = 0; i < mainScene.SpatialObjects.size(); i++)
    {
        mainScene.SpatialObjects[i].SO_mesh.CreateModelMat();
        mainScene.SpatialObjects[i].SO_mesh.CreateRotationMat();
        mainScene.SpatialObjects[i].SO_rigidbody.boundbox.ConstructBoundingBox(mainScene.SpatialObjects[i].SO_mesh);
        vertCount += mainScene.SpatialObjects[i].SO_mesh.vertexes.size();
        indCount += mainScene.SpatialObjects[i].SO_mesh.indices.size();
    }

    player.reset(new Player(30.0f, Camera(glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0,0,-1), 35), glm::vec3(-33,12,-20)));
    player->rotation.x = 300;
    player->rotation.y = 15;
    player->rotation.z = 0;

    shader.CompileShader(ShaderLoc(ReadFile(shaderLoc + "Default.vert"), ReadFile(shaderLoc + "Default.frag")));
    glUseProgram(shader.shader);
    shader.setInt("tex", 0);

    while(run)
    {
        UpdateLogic(window);
        Render(window);
    }

    mainScene.SaveScene(sceneLoc, "main.scene");
    
    texture.Delete();
    SDL_FreeSurface(windowIcon);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    CreateFileLog(logLoc);
    return 0;
}

void UpdateLogic(SDL_Window* window)
{
    SDL_GetWindowSize(window, &currentSCRWIDTH, &currentSCRHEIGHT);

    currentTime = GetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    drawCallAvg = DrawCallCount / (GetTime() / deltaTime);

    for (unsigned int i = 0; i < mainScene.SpatialObjects.size(); i++)
    {
        mainScene.SpatialObjects[i].SO_mesh.CreateModelMat();
        mainScene.SpatialObjects[i].SO_mesh.CreateRotationMat();
    }

    RunSimulation(deltaTime, mainScene);

    player->UpdatePlayer();
    input();
}

void Render(SDL_Window* window)
{
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
    glCullFace(GL_FRONT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    if(showWireFrame)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glUseProgram(shader.shader);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    mainScene.DrawSingle(&shader, player->camera.GetViewMat(), player->camera.GetProjMat(currentSCRWIDTH, currentSCRHEIGHT, 0.1f, 100000.0f), player->position);

    //debuging sets
    SetNeededDebug(player->camera.GetProjMat(currentSCRWIDTH, currentSCRHEIGHT, 0.1f, 100000.0f), player->camera.GetViewMat(), shaderLoc);
    DrawDebugItems();
    if(DebugWindow)
    {
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_GL_SwapWindow(window);
}

float xMouse = 0;
float yMouse = 0;
bool lockMouse = false;
void input()
{
    if(lockMouse)
        SDL_SetRelativeMouseMode(SDL_TRUE);
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
            case SDLK_ESCAPE:
                if(lockMouse)
                {
                    lockMouse = false;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                }
                else
                {
                    lockMouse = true;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                }
                break;
            case SDLK_BACKQUOTE:
                DebugWindow = !DebugWindow;
                break;
            case SDLK_w:
                player->Movement(SDLK_w, deltaTime);
                break;
            case SDLK_s:
                player->Movement(SDLK_s, deltaTime);
                break;
            case SDLK_a:
                player->Movement(SDLK_a, deltaTime);
                break;
            case SDLK_d:
                player->Movement(SDLK_d, deltaTime);
                break;
            case SDLK_SPACE:
                player->Movement(SDLK_SPACE, deltaTime);
                break;
            case SDLK_LSHIFT:
                player->Movement(SDLK_LSHIFT, deltaTime);
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            if(!lockMouse)
                break;
            player->MouseMovement(event.motion.xrel, event.motion.yrel, false, true);
            break;
        }
    }
}


bool ShowSceneViewerMenu = false;
bool ShowObjectViewerMenu = false;
bool ShowConsoleViewerMenu = false;

struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};

void ImguiMenu()
{

    static ScrollingBuffer frameTimes;
    static float HighestFT = 0.0f;
    if(deltaTime > HighestFT)
        HighestFT = deltaTime;
    frameTimes.AddPoint(GetTime(), deltaTime);

    static ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_MenuBar;

    ImGui::Begin("SpaceTesting", nullptr, window_flags);

    ImGui::Text("App avg %.3f ms/frame (%.1f FPS)", deltaTime * 1000, round(1.0f / deltaTime));
    ImGui::Text("%u verts, %u indices (%u tris)", vertCount, indCount, indCount / 3);
    ImGui::Text("Amount of Spatials: (%zu)", mainScene.SpatialObjects.size());
    ImGui::Text("DrawCall Avg: (%.1f) DC/frame, DrawCall Total (%d)", drawCallAvg, DrawCallCount);
    if(platform == "Linux")
        ImGui::Text("Ram Usage: %.2fmb", GetRamUsage() / 1024);
    ImGui::Text("Time Open %.1f minutes", (GetTime() / 60.0f));

    static float frameTimeHistory = 2.75f;
    ImGui::SliderFloat("FrameTimeHistory", &frameTimeHistory, 0.1f, 10.0f);
    if (ImPlot::BeginPlot("##Scrolling", ImVec2(ImGui::GetContentRegionAvail().x,100))) 
    {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1,GetTime() - frameTimeHistory, GetTime(), ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,HighestFT + (HighestFT * 0.25f), ImGuiCond_Always);
        ImPlot::SetNextFillStyle(ImVec4(0,0.5,0.5,1),1.0f);
        ImPlot::PlotShaded("FrameTime", &frameTimes.Data[0].x, &frameTimes.Data[0].y, frameTimes.Data.size(), -INFINITY, 0, frameTimes.Offset, 2 * sizeof(float));
        ImPlot::EndPlot();
    }

    ImGui::Spacing();
    ImGui::Checkbox("Wire Frame", &showWireFrame);
    ImGui::DragFloat3("Player Position", glm::value_ptr(player->position), 1.0f, -50.0f, 50.0f);
    ImGui::DragFloat3("Player Rotation", glm::value_ptr(player->rotation), 1.0f, -360.0f, 360.0f);
    ImGui::SliderFloat("Cam Fov", &player->camera.fov, 179.9f, 0.01f);
    glm::vec3 chunkpos = player->position / glm::vec3(10);
    ImGui::Text("Player in ChunkPos: %dx %dy %dz", (unsigned int)chunkpos.x, (unsigned int)chunkpos.y, (unsigned int)chunkpos.z);

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
            for (unsigned int i = 0; i < mainScene.SpatialObjects.size(); i++)
            {
                if (i == 0)
                    ImGui::SetNextItemOpen(true, ImGuiCond_Once);

                if (ImGui::TreeNode((void*)(intptr_t)i, "Object %d", i))
                {
                    ImGui::DragFloat3("Object Position", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.position), 0.01f, -100000.0f, 100000.0f);
                    ImGui::DragFloat3("Object Rotation", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.rotation), 0.1f, -360.0f, 360.0f);
                    ImGui::DragFloat3("Object Velocity", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.velocity), 0.0f, 0.0f, 0.0f);
                    ImGui::DragFloat3("Object Acceleration", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.acceleration), 0.1f, 100.0f, 100.0f);
                    ImGui::DragFloat3("Object RotationVelocity", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.rotVelocity), 0.0f, 0.0f, 0.0f);
                    ImGui::DragFloat3("Object RotationAcceleration", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.rotAcceleration), 0.1f, 100.0f, 100.0f);
                    ImGui::DragFloat3("Object NetForce", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.totalForce), 0.0f, 0.0f, 0.0f);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::End();
    }

    if(ShowObjectViewerMenu)
    {
        static int counter;
        static glm::vec3 selposition;
        static glm::vec3 selrotation;
        static int IcoSphereSub = 0;
        static char input[128] = "Mesh.obj";
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
        ImGui::Text("%s", GetMeshTypeName(static_cast<MeshType>(counter)).c_str());
        ImGui::InputFloat3("Object Position", glm::value_ptr(selposition));
        ImGui::InputFloat3("Object Rotation", glm::value_ptr(selrotation));
        if(counter == MeshType::IcoSphereMesh)
            ImGui::SliderInt("IcoSphere Subdivison level", &IcoSphereSub, 0, 10);
        if(counter == MeshType::FileMesh)
        {
            ImGui::Text("%s", modelLoc.c_str());
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
                vertCount += selmesh.vertexes.size() * 3;
                indCount += selmesh.indices.size();
                mainScene.AddSpatialObject(selmesh);
                break;
            case IcoSphereMesh:
                selmesh = CreateSphereMesh(selposition, selrotation, IcoSphereSub);
                vertCount += selmesh.vertexes.size() * 3;
                indCount += selmesh.indices.size();
                mainScene.AddSpatialObject(selmesh);
                break;
            case TriangleMesh:
                selmesh = Create2DTriangle(selposition, selrotation);
                vertCount += selmesh.vertexes.size() * 3;
                indCount += selmesh.indices.size();
                mainScene.AddSpatialObject(selmesh);
                break;
            case FileMesh:
                if(!FileExist(modelLoc + input))
                {
                    ImGui::OpenPopup("Error");
                }
                else
                {
                    selmesh = LoadModel(selposition, selrotation, modelLoc + input);
                    vertCount += selmesh.vertexes.size() * 3;
                    indCount += selmesh.indices.size();
                    mainScene.AddSpatialObject(selmesh);
                }
                break;
            }
        }
        if(ImGui::BeginPopup("Error"))
        {
            std::string text = "Model Not Found " + (std::string)input;
            ImGui::Text("%s", text.c_str());
            ImGui::EndPopup();
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