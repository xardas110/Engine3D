#include "VISM2.h"
#include "World.h"
#include "RenderDebugger.h"
#include "Entity.h"
#include "Components.h"
#include "Water.h"

VISM2::~VISM2()
{
}

void VISM2::Create(World* world)
{
    this->world = world;
    world->LoadRenderConfig("../3Dprog22/VISM2RenderConfig.json");
    world->GetRenderCamera()->SetCameraPosition(glm::vec3(0.f, 400.f, 0.f));
    world->GetRenderCamera()->SetFar(5000.f);
    CreateWater(world);
}

void VISM2::CreateWater(World* world)
{
    auto tm = world->GetTextureManager();
    Entity waterEntity = world->CreateEntity("Water");
    auto& water = waterEntity.AddComponent<Water>();
    auto& trans = waterEntity.GetComponent<TransformComponent>();
    trans.SetScale(glm::vec3(10.f));
    trans.SetPosition(glm::vec3(0.f, 200.f, 0.f));
    tm->LoadTexture("../3Dprog22/Assets/Textures/Water/normalmap.png", water.normalmap);
    tm->LoadTexture("../3Dprog22/Assets/Textures/Water/waterDUDV.png", water.waterDUDV);
}

entt::entity VISM2::CreateTerrain(World* world, const std::string& jsonPath)
{
    entt::entity terrainId = world->CreateTerrain(jsonPath);

    Entity ent(terrainId, world); //Lager Terreng
    auto& terrain = ent.GetComponent<TerrainComponent>().terrain;
   
    //Laster opp terrengets heightmap til fysikk systemet for kollisjon
    CollisionHeightmap heightmap; auto& config = terrain.config;
    glm::vec3 scale(config.scaleXZ, config.scaleY, config.scaleXZ);
    heightmap.SetHeightmap(config.N, config.N, scale, &terrain.heightmapBuffer);
    world->physicsSystem->RegisterHeightmap(heightmap);

    return terrainId;
}

void VISM2::UpdateEditor(World* world, float deltatime)
{
    ImGui::Begin("VISM2 Oppgaver");
    ImGui::Text("Task 2.5 - Visualize terrain");
    ImGui::Checkbox("Terrain Wireframe", &world->terrainSystem.bShowWireframe);

    if (ImGui::Button("Create HighRes terrain"))
    {
        world->DeleteTerrain(terrainLowRes);
        world->DeleteTerrain(terrainHighRes);
        
        terrainHighRes = CreateTerrain(world, "../3Dprog22/VISM2TerrainHighRes.json");
    }
    if (ImGui::Button("Create LowRes terrain"))
    {
        world->DeleteTerrain(terrainLowRes);
        world->DeleteTerrain(terrainHighRes);

        terrainLowRes = CreateTerrain(world, "../3Dprog22/VISM2TerrainLowRes.json");
    }

    ImGui::Text("\nTask 2.6 - Visualize Heightlines");
    ImGui::Text("Red lines: 5m");
    ImGui::Text("White lines: 100m");
    ImGui::Checkbox("Display heightlines", &world->terrainSystem.bShowHeightlines);

    ImGui::Text("\nTask 3.1 - Spawn Ball (Press R)");
    ImGui::Text("Task 3.2 - Spawn Balls");
    ImGui::Checkbox("Cast Shadow on rain", &bCastShadowOnRain);
    world->physicsSystem->SetCastShadowOnPhysicBalls(bCastShadowOnRain);
    if (ImGui::Button("Spawn Rain"))
    {
        ClearBalls();
        SpawnBalls();
    }
    ImGui::SliderInt("Num Balls X", &numBallsX, 10, 200);
    ImGui::SliderInt("Num Balls Y", &numBallsZ, 10, 200);
    ImGui::SliderInt("Ball Spacing", &ballSpacing, 10, 100);
    ImGui::SliderFloat3("External Impulse", &world->physicsSystem->externalImpulse.x, -100.f, 100.f);

    ImGui::Text("\nTask 3.3 - Simulate B-Spline");
    if (ImGui::Button("Spawn Rain with B-Splines"))
    {
        ClearBalls();
        SpawnBalls(true);
    }

    ImGui::Checkbox("Enable Multithreading(32 threads)", &world->physicsSystem->bSimulateThreaded);
    
    if (ImGui::Button("Clear Balls"))
    {
        ClearBalls();
    }

    ImGui::Checkbox("Draw Octree", &bDrawOctreeLeafs);
    ImGui::Checkbox("Draw bounding boxes", &bDrawBoundingBoxes);
    ImGui::Checkbox("Draw Convex Hulls", &bDrawConvexHulls);

    if (bDrawOctreeLeafs) world->physicsSystem->GetOctree()->DrawLeafBounds();
    if (bDrawBoundingBoxes) world->physicsSystem->DrawBoundingBoxes();
    if (bDrawConvexHulls) world->physicsSystem->DrawConvexHulls();

    ImGui::End();
}
void VISM2::BeginPlay(World* world)
{
}

void VISM2::EndPlay(World* world)
{
}

void VISM2::Update(World* world, float deltatime)
{
}
void VISM2::Reset(World* world)
{
}
void VISM2::OnKeyPress(QKeyEvent* event)
{
    auto* cam = world->GetRenderCamera();

    if (Qt::Key_R == event->key())
    {
        SpawnBall(cam->GetPosition() + cam->GetDirection() * 10.f);
    }
}

void VISM2::SpawnBall(const glm::vec3& pos, bool bHighres, bool bAddSpline)
{
    auto sm = world->GetStaticMeshManager();

    Entity e = world->CreateEntity("Ball" + std::to_string(nextNameId++));
    balls.emplace_back(e.GetEntityId());

    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::Sphere).col;
    auto& sphereBody = e.AddComponent<PhysicsComponent>().body;

    if (bAddSpline)
    {
        e.AddComponent<BSplines>();
    }

    if (bHighres)
    { 
        auto& sphereMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere_LOD4.obj", sphereMesh);
        sphereBody.SetMass(1000.f);
    }
    else
    {
        e.AddComponent<PhysicsBall>();
        sphereBody.SetMass(1.f);
    }
    
    sphereBody.SetFriction(0.5f);

    collider.SetExtents(glm::vec3(5.2f)); 
   
    e.SetPosition(pos);
    e.SetScale({ 5.f, 5.f, 5.f }); 
}

void VISM2::SpawnBalls(bool bAddSpline)
{
    int iterations = 1;
    int nrRows = numBallsX;
    int nrColumns = numBallsZ;
    float spacing = ballSpacing;
    for (auto f = 0; f < iterations; f++)
        for (auto i = 0; i < nrRows; i++)
        {
            for (auto j = 0; j < nrColumns; j++)
            {
                SpawnBall({ i * spacing - (nrRows * spacing * 0.5f), 1500.f * (f + 1), j * spacing - (nrColumns * spacing * 0.5f) }, false, bAddSpline);
            }
        };
}

void VISM2::ClearBalls()
{
    for (auto entity : balls)
    {
        world->DeleteEntity(entity);
    }
    balls.clear();
    nextNameId = 0;
}


