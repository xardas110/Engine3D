#include "World.h"
#include "Entity.h"
#include "Include/glm/glm.hpp"
#include "Include/glm/gtc/matrix_transform.hpp"
#include "Include/glm/gtc/type_ptr.hpp">"
#include "Components.h"
#include "DirectionalLight.h"
#include <Array>
#include <iostream>

#include "AiCharacter.h"
#include "PointGenerator.h"
#include "Debug.h"
#include "Character.h"
#include "Include/glm/gtx/euler_angles.hpp"
#include "Include/glm/gtx/vec_swizzle.hpp"
#include "Components.h"
#include "Trophy.h"
#include "AudioManager.h"
#include "MaterialManager.h"
#include "RenderEngine.h"
#include "DeferredRenderer.h"
#include <random>
#include "RenderDebugger.h"
#include "GrassGameMode.h"
#include "PhysicsGameMode.h"
#include "Sponza.h"
#include "VISM2.h"
#include <execution>

World* world = nullptr;
size_t World::nameVal = 0;

World::World() : worldSerializer(this)
{
}

World::~World()
{
    delete deferedRenderer;
    deferedRenderer = nullptr;

    delete meshManager;
    meshManager = nullptr;

    delete textureManager;
    textureManager = nullptr;

    delete gameMode;
    gameMode = nullptr;

    delete physicsSystem;
    physicsSystem = nullptr;
}

DeferredRendererConfig& World::GetRenderConfig()
{
    return deferedRenderer->config;
}

void World::OnInit(Camera* renderCamera)
{
    srand(time(nullptr));

    world = this;
    textureManager = new TextureManager();
    meshManager = new MeshManager();
    deferedRenderer = new DeferredRenderer;
    physicsSystem = new PhysicsSystem(this, entRegistry);

    physicsSystem->Init();
    pathFindingSystem.Init(this, entRegistry);

    SetRenderCamera(renderCamera);
 
    deferedRenderer->Init(this);
    cameraSystem.Init(this, entRegistry);
    audioSystem.Init(this, entRegistry);
    skySystem.Init(this, entRegistry);

    Create();

    if (gameMode) gameMode->Create(this, entRegistry);
    if (gameMode) gameMode->Create(this);
        
    staticMeshSystem.Init(this, entRegistry);
    skeletalMeshSystem.Init(this, entRegistry);
    staticMeshInstancedSystem.Init(this, entRegistry);
    vegetationSystem.Init(this, entRegistry);
    billboardSystem.Init(this, entRegistry);

    InitScripts();

    particleSystem.Init(this, entRegistry);
    bSplineSystem.Init(this, entRegistry);
}

void World::OnUpdate(float deltaTime)
{
    this->deltatime = deltaTime;

    bSplineSystem.PreUpdate(deltatime);

    Update(deltaTime);
    
    if (WorldState::Play == worldState)
    {
        if (gameMode) gameMode->Update(this, entRegistry, deltaTime);
        if (gameMode) gameMode->Update(this, deltaTime);        
    }

    //Oppgave 6
    if (IsState(WorldState::Editor))
    {
        if (worldSettings.bShowWorldAxises)
            UpdateWorldAxises();
    }

    if (gameMode) gameMode->UpdateEditor(this, entRegistry, deltaTime);
    if (gameMode) gameMode->UpdateEditor(this, deltaTime);

    cameraSystem.Update(this, entRegistry, deltaTime);
    audioSystem.Update(this, entRegistry);
    skySystem.Update(this, entRegistry, deltaTime);
    terrainSystem.Update(this, entRegistry);
    UpdateScripts(deltaTime);

    if (WorldState::Play == worldState);
        skeletalMeshSystem.Update(this, entRegistry, deltaTime);

    staticMeshInstancedSystem.Update(this, entRegistry);
    vegetationSystem.Update(this, entRegistry);
    particleSystem.Update(this, entRegistry, deltaTime);

    billboardSystem.Update(this, entRegistry, deltaTime);

    UpdateDeferredEntityDeletion();

    if (physicsSystem) physicsSystem->Update(deltatime);

    pathFindingSystem.Update(this, entRegistry);

    bSplineSystem.PostUpdate(deltatime);
}

void World::DrawDebugSettings(float deltatime)
{
    auto* rd = RenderDebugger::Get();

    if (worldSettings.bShowPathfindingOccupied)
    {
        pathFindingSystem.DrawDebugPF1();
    }
    static bool bShow{false}; // for better performance, cant redraw each frame
    if (worldSettings.bShowPathfinding)
    {
        if (!bShow)
        {      
            pathFindingSystem.DrawDebugPF2();
            bShow = true;
        }
    }
    else
    {
        if (bShow)
            rd->ClearContainers();

        bShow = false;
    }
}

void World::SetState(WorldState::State newState)
{
    worldState = newState;
}

float World::GetDeltaTime()
{
   return Get()->deltatime;
}

WorldState::State World::GetState() const
{
    return worldState;
}

bool World::IsState(WorldState::State state) const
{
    return state == worldState;
}

bool World::IsNotState(WorldState::State state) const
{
    return state != worldState;
}

size_t World::GetNextNameVal()
{
    return ++nameVal;
}

glm::vec3 World::GetWorldUp()
{
    return {0.f, 1.f, 0.f};
}

World* World::Get()
{
    return world;
}

glm::vec3 World::GetGravity()
{
    return glm::vec3(0.f, -60.82f, 0.f);
}

void World::SetAmbientSound(AudioData* audio)
{
    ambientSound = audio;
}

AudioData* World::GetAmbientSound()
{
    return ambientSound;
}

void World::on_saveButton_clicked()
{
    worldSerializer.Serialize("../3Dprog22/Assets/World/World1");
}

void World::on_loadButton_clicked()
{
    worldSerializer.Deserialize("../3Dprog22/Assets/World/World1.json");
}

void World::SaveWorld(const std::string& path)
{
    worldSerializer.Serialize(path);
}

void World::LoadWorld(const std::string& path)
{
    worldSerializer.Deserialize(path);
}

entt::entity World::CreateTerrain(const std::string& configJsonPath)
{
    return terrainSystem.CreateTerrain(this, entRegistry, configJsonPath);
}

entt::entity World::CreateTerrain(Texture& heightmap, const std::string& configJsonPath)
{
    return terrainSystem.CreateTerrain(this, entRegistry, heightmap, configJsonPath);
}

void World::DeleteTerrain(entt::entity entity)
{
    terrainSystem.DeleteTerrain(this, entity);
}

void World::InitScripts()
{
    entRegistry.view<ScriptComponent>().each([=](auto entity, auto& sc)
        {	
            sc.instance = sc.InstantiateScript();
            sc.instance->id = entity;
            sc.instance->world = this;
            sc.instance->OnCreate();        	
        });
}

void World::DestroyScripts()
{
    entRegistry.view<ScriptComponent>().each([=](auto entity, auto& sc)
        {
            if (entRegistry.valid(entity))
                sc.DestroyScript(&sc);
        });
}

void World::InitBeginPlayScripts()
{
    entRegistry.view<ScriptComponent>().each([=](auto entity, auto& sc)
        {
            sc.instance->id = entity;
            sc.instance->world = this;
            sc.instance->OnBeginPlay();
        });
}

void World::UpdateCameraScripts(float deltaTime)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<CameraComponent>().each([=](auto entity, auto& sc)
        {
            sc.camera.UpdateCamera(deltaTime);
        });
}

void World::UpdateScriptComponents(float deltaTime)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<ScriptComponent>().each([=](auto entity, auto& sc)
        {
            sc.instance->OnUpdate(deltaTime);
        });
}

void World::PreLoadAssets(const std::string& jsonPath)
{
    std::cout << "Loading static asset list" << std::endl;
    Json json = Json::Load(jsonPath);
    std::cout << "Done loading.." << std::endl;

    std::vector<std::string> paths;

    for (auto i = 0; i < json["paths"].Size(); i++)
    {
        paths.push_back(json["paths"][i]);
    }

    GetStaticMeshManager()->LoadStaticMeshesAsync(paths);
}

void World::UpdateMousePressedScripts(QMouseEvent* event)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<InputComponent>().each([=](auto entity, auto& ic)
        {
    		if (ic.mousePressedEvent) ic.mousePressedEvent(event);
        });
}

void World::UpdateMouseReleasedScripts(QMouseEvent* event)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<InputComponent>().each([=](auto entity, auto& ic)
        {
            if (ic.mouseReleasedEvent) ic.mouseReleasedEvent(event);
        });
}

void World::UpdateMouseMoveScripts(QMouseEvent* event)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<InputComponent>().each([=](auto entity, auto& ic)
        {
            if (ic.mouseMoveEvent) ic.mouseMoveEvent(event);
        });
}

void World::UpdateMouseHeldScripts(std::map<int, bool>* mouseHeld, float deltaTime)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<InputComponent>().each([=](auto entity, auto& ic)
        {
            if (ic.mouseHeldEvent) ic.mouseHeldEvent(mouseHeld, deltaTime);
        });
}

void World::UpdateKeyPressedScripts(QKeyEvent* event)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<InputComponent>().each([=](auto entity, auto& ic)
        {
            if (ic.keyPressedEvent) ic.keyPressedEvent(event);
        });
}

void World::UpdateKeyReleasedScripts(QKeyEvent* event)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<InputComponent>().each([=](auto entity, auto& ic)
        {
            if (ic.keyReleasedEvent) ic.keyReleasedEvent(event);
        });
}

void World::UpdateKeyHeldScripts(std::map<int, bool>* keyHeld, float deltaTime)
{
    if (WorldState::Play != worldState) return;
	
    entRegistry.view<InputComponent>().each([=](auto entity, auto& ic)
        {
            if (ic.keyHeldEvent) ic.keyHeldEvent(keyHeld, deltaTime);
        });
}

void World::UpdateScripts(float deltaTime)
{
    UpdateCameraScripts(deltaTime);
    UpdateScriptComponents(deltaTime);
}

void World::UpdateDeferredEntityDeletion()
{
	for (auto entity : entityForDeletion)
	{
		if (entRegistry.valid(entity))
		{
            Entity ent(entity, this);
            ent.Delete();
			entRegistry.destroy(entity);
        }
	}
    entityForDeletion.clear();
}

float World::GetTerrainHeightAt(float x, float z)
{
    auto view = entRegistry.view<TerrainComponent>();

    for (auto entity : view)
    {
        auto& terrain = entRegistry.get<TerrainComponent>(entity).terrain;
        return terrain.GetHeightAt(x,z);
    }

    return 0.0f;
}

float World::GetTerrainSlopeAt(float x, float z)
{
    auto view = entRegistry.view<TerrainComponent>();

    for (auto entity : view)
    {
        auto& terrain = entRegistry.get<TerrainComponent>(entity).terrain;
        return terrain.GetNormalSlopeAt(x, z);
    }

    return 0.0f;
}

glm::vec3 World::GetTerrainNormalAt(float x, float z)
{
    auto view = entRegistry.view<TerrainComponent>();

    for (auto entity : view)
    {
        auto& terrain = entRegistry.get<TerrainComponent>(entity).terrain;
        return terrain.GetNormalAt(x, z);
    }

    return glm::vec3();
}

float World::GetElapsedTimeInSeconds()
{
    static std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    static std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    auto val = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
    end = std::chrono::steady_clock::now();

    return val;
}

TerrainConfig* World::GetTerrainConfig()
{
    auto view = entRegistry.view<TerrainComponent>();

    for (auto entity : view)
    {
        auto& terrain = entRegistry.get<TerrainComponent>(entity).terrain;

        return &terrain.config;
    }

    return nullptr;
}

Terrain* World::GetTerrain()
{
    auto view = entRegistry.view<TerrainComponent>();

    for (auto entity : view)
    {
        return &entRegistry.get<TerrainComponent>(entity).terrain;
    }

    return nullptr;
}

Entity World::CreateEntity(const std::string& tag)
{
    Entity ent{ entRegistry.create(), this };
    ent.AddComponent<TransformComponent>();
    ent.AddComponent<TagComponent>(tag);
    return ent;
}

void World::UpdateWorldAxises()
{
    auto* rd = RenderDebugger::Get();

    glm::vec3 worldOrig{ 0.f };
    float length = 128.f; 

    rd->AddDebugSegment(0.f, worldOrig, {length, 0.f, 0.f});
    rd->AddDebugSegment(0.f, worldOrig, { 0.f,length, 0.f }, {0.f, 1.f, 0.f});
    rd->AddDebugSegment(0.f, worldOrig, { 0.f, 0.f, length }, { 0.f, 0.f, 1.f });
}

void World::OnBeginPlay()
{
    SetState(WorldState::Play);

    //if (GetAmbientSound())
    //    if (GetAmbientSound()->IsValid());
    //        GetAmbientSound()->Play();

    BeginPlay();

    if (gameMode) gameMode->BeginPlay(this, entRegistry);
    if (gameMode) gameMode->BeginPlay(this);

    InitBeginPlayScripts();  
}

void World::OnEndPlay()
{
    SetState(WorldState::Editor);
   // GetAmbientSound()->Stop();
    EndPlay();

    if (gameMode) gameMode->EndPlay(this, entRegistry);
    if (gameMode) gameMode->EndPlay(this);
}

void World::OnRender(float deltaTime)
{
    DrawDebugSettings(deltatime);
    deferedRenderer->Render(this, deltaTime);
}

void World::LoadRenderConfig(const std::string& jsonPath)
{
    deferedRenderer->LoadRenderConfig(jsonPath);

    DirLight light;
    GetDirectionalLight(light);

    auto& c = deferedRenderer->config;

    light.lightDir = c.dirlight.direction;
    light.ambient = c.dirlight.ambient;
    light.diffuse = c.dirlight.diffuse;
    light.specular = c.dirlight.specular;
    light.itensity = c.dirlight.itensity;
}

bool World::GetDirectionalLight(DirLight& outDirlight)
{
    outDirlight = skySystem.GetDirectionalLight();
    return true;
}

void World::DeleteEntity(entt::entity id)
{
    entityForDeletion.emplace_back(id);
}

void World::DeleteEntityInstant(entt::entity id)
{
    entRegistry.destroy(id);
}

void World::DeleteEntities(std::initializer_list<std::string> stringFlags)
{
    entRegistry.each(
        [this, stringFlags](const auto entity)
        {
            Entity current(entity, this);

            if (!current.IsValid()) return;

            auto& tag = current.GetComponent<TagComponent>().tag;

            for (auto& flag : stringFlags)
            {
                if (tag == flag) continue;

                current.Delete();
                entRegistry.destroy(entity);
            }      
        }
    );
}

Camera* World::GetRenderCamera() const
{
    return renderCamera;
}

void World::OnMouseReleasedFromRenderWindow(QMouseEvent* event)
{
    MouseReleased(event);
    UpdateMouseReleasedScripts(event);
}

void World::OnMouseMoveFromRenderWindow(QMouseEvent* event)
{
    MouseMoved(event);
    UpdateMouseMoveScripts(event);
}

void World::OnMouseButtonHeldFromRenderWindow(std::map<int, bool>* mouseHeld, float deltaTime)
{
    UpdateMouseHeldScripts(mouseHeld, deltaTime);
}

void World::OnExposeEvent(QExposeEvent* e)
{
    deferedRenderer->OnResize();
}

void World::OnKeyReleasedFromRenderWindow(QKeyEvent* event)
{
    KeyReleased(event);
    UpdateKeyReleasedScripts(event);
}

void World::OnKeysHeld(std::map<int, bool>* keysHeld, float deltaTime)
{
    UpdateKeyHeldScripts(keysHeld, deltaTime);
}

void World::SetRenderCamera(Camera* renderCamera)
{
    this->renderCamera = renderCamera;
}

SkeletalMeshManager* World::GetSkeletalMeshManager()
{
    return &skeletalMeshManager;
}

StaticMeshManager* World::GetStaticMeshManager()
{
    return &staticMeshManager;
}

MeshManager* World::GetMeshManager()
{
    return meshManager;
}

TextureManager* World::GetTextureManager()
{
    return textureManager;
}

const WorldSettings& World::GetWorldSettings() const
{
    return worldSettings;
}

void World::SetGameMode(GameMode* gm)
{
    auto* rd = RenderDebugger::Get();

    delete physicsSystem;
    physicsSystem = nullptr;

    audioSystem.DeleteAll(entRegistry);
    DeleteEntities({""});
    rd->ClearContainers();

    if (gameMode)
    {
        delete gameMode;
        gameMode = nullptr;
        DestroyScripts();
    }

    if (!gm) return;

    physicsSystem = new PhysicsSystem(world, entRegistry);

    gameMode = gm;
    gameMode->Create(this, entRegistry);  
    gameMode->Create(this);
    InitScripts();

    physicsSystem->Init();
    pathFindingSystem.Init(this, entRegistry);

    if (IsState(WorldState::Play)) OnBeginPlay();

    deferedRenderer->ReloadVoxels(this);
}

void World::StartGameMode(int index)
{
    switch (index)
    {
        case 0:
        {
            SetGameMode(nullptr);
        }
        break;
        case 1:
        {
            SetGameMode(new EksamenGameMode);
        }
        break;
        case 2:
        {
            SetGameMode(new GrassGameMode);
        }
        break;
        case 3:
        {
            SetGameMode(new PhysicsGameMode);
        }
        break;
        case 4:
        {
            SetGameMode(new Sponza);
        }
        break;
        case 5:
        {
            SetGameMode(new VISM2);
        }
        break;
    default:
        break;
    }
}

void World::OnKeyPressedFromRenderWindow(QKeyEvent* event)
{
    KeyPressed(event);
    UpdateKeyPressedScripts(event);  

    if (gameMode) gameMode->OnKeyPress(event);
}

void World::OnMousePressedFromRenderWindow(QMouseEvent* event)
{
    MousePressed(event);
    UpdateMousePressedScripts(event); 
}

