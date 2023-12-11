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
#include <assert.h>
#include <algorithm>

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
int runningSim = 1;
bool showWireFrame = false;
float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastTime = 0.0f;
double updateTime = 0.0;
double updateFixedTime = 0.0;
int currentSCRWIDTH = 0;
int currentSCRHEIGHT = 0;
bool vsync = true;
SDL_Surface *windowIcon;
ImGuiIO *io;
std::string platform;

const std::string imageLoc = "res/Textures/";
const std::string shaderLoc = "res/Shaders/";
const std::string modelLoc = "res/Models/";
const std::string logLoc = "res/Logs/";
const std::string sceneLoc = "res/Scenes/";

void UpdateLogic(SDL_Window* window);
void FixedUpdateLogic();
void Render(SDL_Window* window);
void ImguiMenu();
void input();
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data);

Shader shader;
Shader depthShader;
Texture texture;
std::unique_ptr<Player> player;
Scene mainScene;
unsigned int vertCount, indCount;
unsigned int depthMap;
unsigned int depthMapFBO;

int run = 1;
int main()
{
    
    //Sdl creation
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        if(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", "Could Not Init SDL Video", NULL) == -1)
        {
            assert(!SDL_Init(SDL_INIT_VIDEO));
        }
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    
    SDL_Window* window = SDL_CreateWindow("Spatial Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCRWIDTH, SCRHEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(vsync);
    if(!gladLoadGL())
    {
        if(SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "OpenGL Error", "Could Not Load OpenGL", NULL) == -1)
        {
            assert(gladLoadGL());
        }
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLDebugMessageCallback, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

    windowIcon = IMG_Load((imageLoc + "/IconSpace.png").c_str());
    SDL_SetWindowIcon(window, windowIcon);

    platform = SDL_GetPlatform();

    //ImGui Creation
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& IO = ImGui::GetIO();
    io = &IO;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();
    ImGui::SetNextWindowSize(ImVec2(450,420), ImGuiCond_FirstUseEver);


    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, ShadowSize, ShadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    //scene initilzation
    //LoadScene(sceneLoc, "main.scene", mainScene);
    texture.LoadTexture(imageLoc + "RedDebug.png");

    unsigned int count = 0;
    for (int i = 0; i < 1; i++)
    {
        for (int g = 0; g < 1; g++)
        {
            mainScene.AddSpatialObject(LoadModel(glm::vec3(i * 100, -0.7f, g * 100), glm::vec3(0,0,0), modelLoc + "Floor.obj"));
            mainScene.SpatialObjects[count].SO_rigidbody.isStatic = true;
            count++;
        }
    }

    mainScene.AddSpatialObject(CreateSpikerMesh(glm::vec3(0,5,0), glm::vec3(0,0,0), 0.3f, 2));
    //mainScene.AddSpatialObject(LoadModel(glm::vec3(0,5,0), glm::vec3(0,0,0), modelLoc + "Bunny.obj"));
    /*mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,5,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,10,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,15,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,20,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,25,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,30,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,35,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,40,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,45,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,50,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,55,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,60,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,65,0), glm::vec3(0,0,0), 2));
    mainScene.AddSpatialObject(CreateSphereMesh(glm::vec3(0,70,0), glm::vec3(0,0,0), 2));*/
    
    for (int i = -48 ; i < 48 + (100 * 0); i += 3)
    {
        for (int g = -48; g < 48 + (100 * 0); g += 3)
        {
            for (int j = 0; j < 3; j++)
            {
                //mainScene.AddSpatialObject(LoadModel(glm::vec3(i,3 + 5 * j,g), glm::vec3(0,0,0), modelLoc + "Cube.obj"));
            }
        }
    }

    for (unsigned int i = 0; i < mainScene.SpatialObjects.size(); i++)
    {
        vertCount += mainScene.SpatialObjects[i].SO_mesh.vertexes.size();
        indCount += mainScene.SpatialObjects[i].SO_mesh.indices.size();
    }

    player.reset(new Player(30.0f, Camera(glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0,0,-1), 60), glm::vec3(-33,12,-20)));
    player->rotation.x = 300;
    player->rotation.y = 15;
    player->rotation.z = 0;

    shader.CompileShader(ShaderLoc(ReadFile(shaderLoc + "Default.vert"), ReadFile(shaderLoc + "Default.frag")));
    glUseProgram(shader.shader);
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);
    depthShader.CompileShader(ShaderLoc(ReadFile(shaderLoc + "Depth.vert"), ReadFile(shaderLoc + "Depth.frag")));

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
    updateTime = SDL_GetTicks64();
    SDL_GetWindowSize(window, &currentSCRWIDTH, &currentSCRHEIGHT);

    currentTime = GetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    drawCallAvg = DrawCallCount / (GetTime() / 1.0f / deltaTime);

    for (unsigned int i = 0; i < mainScene.SpatialObjects.size(); i++)
    {
        mainScene.SpatialObjects[i].SO_mesh.CreateModelMat();
        mainScene.SpatialObjects[i].SO_mesh.CreateRotationMat();
    }

    updateTime -= SDL_GetTicks64();

    updateFixedTime = SDL_GetTicks64();
    static float totalTime;
    int counter = 0;
    totalTime += deltaTime;
    while (totalTime >= PhysicsStep)
    {
        FixedUpdateLogic();
        counter++;

        if(deltaTime >= PhysicsStep)
        {
            if(counter == maxPhysicSteps)
                break;
            totalTime -= PhysicsStep;
        }
        else
        {
            totalTime = 0;
            break;
        }
    }
    updateFixedTime -= SDL_GetTicks64();

    input();
}

void FixedUpdateLogic()
{
    RunSimulation(deltaTime, mainScene, runningSim);
    player->UpdatePlayer(PhysicsStep);
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

    glClearColor(0.4f, 0.7f, 0.8f, 1.0f);
    
    glm::vec3 lightPos = glm::vec3(sin(GetTime()  / 10.0f) * 70, 80.0f, cos(GetTime()  / 10.0f) * 70);
    float objectDists[mainScene.SpatialObjects.size()];
    for (unsigned int i = 0; i < mainScene.SpatialObjects.size(); i++)
    {
        objectDists[i] = glm::distance(lightPos, mainScene.SpatialObjects[i].SO_rigidbody.position);
    }
    std::sort(objectDists, objectDists + mainScene.SpatialObjects.size());

    float near_plane = objectDists[0] * 0.7f, far_plane = objectDists[mainScene.SpatialObjects.size() - 1] + 5.0f;
    float fov = glm::degrees(2.0f * atan(objectDists[mainScene.SpatialObjects.size() - 1] / far_plane));
    glm::mat4 lightProjection = glm::perspective(glm::radians(fov), (float)ShadowSize / (float)ShadowSize, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    glUseProgram(depthShader.shader);
    depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, ShadowSize, ShadowSize);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    mainScene.DrawSingleNoAssign(&depthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, SCRWIDTH, SCRHEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    //glCullFace(GL_FRONT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    if(showWireFrame)
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glUseProgram(shader.shader);
    shader.setVec3("lightPos", lightPos);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    mainScene.DrawSingle(&shader, lightSpaceMatrix, player->camera.GetViewMat(), player->camera.GetProjMat(currentSCRWIDTH, currentSCRHEIGHT, 0.1f, 100000.0f), player->position);

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
                player->Movement(SDLK_w);
                break;
            case SDLK_s:
                player->Movement(SDLK_s);
                break;
            case SDLK_a:
                player->Movement(SDLK_a);
                break;
            case SDLK_d:
                player->Movement(SDLK_d);
                break;
            case SDLK_SPACE:
                player->Movement(SDLK_SPACE);
                break;
            case SDLK_LSHIFT:
                player->Movement(SDLK_LSHIFT);
                break;
            case SDLK_t:
                runningSim *= -1;
                break;
            case SDLK_y:
                RunSimulation(deltaTime, mainScene, 1);
                break;
            case SDLK_v:
                player->LaunchObject(mainScene, modelLoc + "Cube.obj");
                break;
            case SDLK_b:
                player->LaunchObject(mainScene, modelLoc + "Floor.obj");
                break;
            case SDLK_c:
                player->LaunchObject(mainScene, modelLoc + "Monkey.obj");
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

struct ScrollingBuffer 
{
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) 
    {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) 
    {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() 
    {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};

void ImguiMenu()
{

    static ScrollingBuffer frameTimes(10000);
    static float HighestFT = 0.0f;
    if(deltaTime > HighestFT)
        HighestFT = deltaTime;
    frameTimes.AddPoint(GetTime(), deltaTime);

    static ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_MenuBar;

    ImGui::Begin("SpaceTesting", nullptr, window_flags);

    //needs to be io.framerate because the actal deltatime is polled too fast and the 
    //result is hard to read
    ImGui::Text("Version %s", EngVer.c_str());
    ImGui::Text("%.3f ms/frame (%.1f FPS)", (1.0f / io->Framerate) * 1000.0f, io->Framerate);
    ImGui::Text("%u verts, %u indices (%u tris)", vertCount, indCount, indCount / 3);
    ImGui::Text("Amount of Spatials: (%zu)", mainScene.SpatialObjects.size());
    ImGui::Text("DrawCall Avg: (%.1f) DC/frame, DrawCall Total (%d)", drawCallAvg, DrawCallCount);
    if(platform == "Linux")
        ImGui::Text("Ram Usage: %.2fmb", GetRamUsage() / 1024);
    ImGui::Text("Time Open %.1f minutes", (GetTime() / 60.0f));
    ImGui::Text("Time taken for Update run %.2fms ", fabs(updateTime));
    ImGui::Text("Time taken for Fixed Update run %.2fms ", fabs(updateFixedTime));

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
    ImGui::Checkbox("Wire Frame", &showWireFrame);
    if(ImGui::Checkbox("Vsync", &vsync))
    {
        SDL_GL_SetSwapInterval(vsync);
    }

    ImGui::Spacing();
    ImGui::DragFloat("Physics Speed", &PhysicsSpeed, 0.01f, -10.0f, 10.0f);
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
                if (ImGui::TreeNode((void*)(intptr_t)i, "Object %d", mainScene.SpatialObjects[i].SO_id))
                {
                    ImGui::DragFloat3("Object Position", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.position), 0.05f, -100000.0f, 100000.0f);
                    ImGui::DragFloat3("Object Rotation", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.rotation), 0.1f, -360.0f, 360.0f);
                    ImGui::DragFloat3("Object Velocity", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.velocity), 0.5f, -1000.0f, 1000.0f);
                    ImGui::DragFloat3("Object Acceleration", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.acceleration), 0.1f, -100.0f, 100.0f);
                    ImGui::DragFloat3("Object RotationVelocity", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.rotVelocity), 0.5f, -1000.0f, 10000000.0f);
                    ImGui::DragFloat3("Object RotationAcceleration", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.rotAcceleration), 0.1f, -100.0f, 100.0f);
                    ImGui::DragFloat3("Object NetForce", glm::value_ptr(mainScene.SpatialObjects[i].SO_rigidbody.totalForce), 0.1f, -100.0f, 100.0f);
                    ImGui::Text("Object is experiencing %.1fg's", mainScene.SpatialObjects[i].SO_rigidbody.gForce);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Scenes"))
        {
            static std::vector<std::string> files = GetFiles(sceneLoc);
            for (unsigned int i = 0; i < files.size(); i++)
            {
                if (ImGui::TreeNode((void*)(intptr_t)i, "Scene: %s", files[i].c_str()))
                {
                    ImGui::SameLine();
                    if(ImGui::Button("Load"))
                    {
                        LoadScene(sceneLoc, files[i], mainScene);
                    }
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
        static glm::vec3 selvelocity;
        static glm::vec3 selacceleration;
        static glm::vec3 selvelocityrot;
        static glm::vec3 selaccelerationrot;
        static int IcoSphereSub = 0;
        static std::string input;
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
        if(counter == MeshType::IcoSphereMesh)
            ImGui::SliderInt("IcoSphere Subdivison level", &IcoSphereSub, 0, 10);
        if(counter == MeshType::FileMesh)
        {
            if(ImGui::TreeNode("Models"))
            {
                static std::vector<std::string> files = GetFiles(modelLoc);
                for (unsigned int i = 0; i < files.size(); i++)
                {
                    if (i == 0)
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

                    if (ImGui::TreeNode((void*)(intptr_t)i, "Model: %s", files[i].c_str()))
                    {
                        ImGui::SameLine();
                        if(ImGui::Button("Set"))
                        {
                            input = files[i];
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
        else
        {
            input = GetMeshTypeName(static_cast<MeshType>(counter));
        }
        ImGui::InputFloat3("Object Position", glm::value_ptr(selposition));
        ImGui::InputFloat3("Object Rotation", glm::value_ptr(selrotation));
        ImGui::InputFloat3("Object Velocity", glm::value_ptr(selvelocity));
        ImGui::InputFloat3("Object Acceleration", glm::value_ptr(selacceleration));
        ImGui::InputFloat3("Object Rotation Velocity", glm::value_ptr(selvelocityrot));
        ImGui::InputFloat3("Object Rotation Acceleration", glm::value_ptr(selaccelerationrot));

        ImGui::Text("Current Model: %s", input.c_str());

        if(ImGui::Button("Add Object"))
        {
            Mesh selmesh = Mesh();
            switch (static_cast<MeshType>(counter))
            {
            case CubeMesh:
                selmesh = CreateCubeMesh(selposition, selrotation);
                vertCount += selmesh.vertexes.size();
                indCount += selmesh.indices.size();
                mainScene.AddSpatialObject(selmesh);
                break;
            case IcoSphereMesh:
                selmesh = CreateSphereMesh(selposition, selrotation, IcoSphereSub);
                vertCount += selmesh.vertexes.size();
                indCount += selmesh.indices.size();
                mainScene.AddSpatialObject(selmesh);
                break;
            case TriangleMesh:
                selmesh = Create2DTriangle(selposition, selrotation);
                vertCount += selmesh.vertexes.size();
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
                    unsigned int id = mainScene.SpatialObjects.size();
                    mainScene.AddSpatialObject(LoadModel(selposition, selrotation, modelLoc + input));
                    vertCount += mainScene.SpatialObjects[id].SO_mesh.vertexes.size();
                    indCount += mainScene.SpatialObjects[id].SO_mesh.indices.size();
                    mainScene.SpatialObjects[id].SO_rigidbody.velocity = selvelocity;
                    mainScene.SpatialObjects[id].SO_rigidbody.acceleration = selacceleration;
                    mainScene.SpatialObjects[id].SO_rigidbody.rotVelocity = selvelocityrot;
                    mainScene.SpatialObjects[id].SO_rigidbody.rotAcceleration = selaccelerationrot;
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

void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

        case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

        default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

        case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

        case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

        default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

        case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

        default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n",
            id, _type.c_str(), _severity.c_str(), _source.c_str(), msg);
}
