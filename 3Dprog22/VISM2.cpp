#include "VISM2.h"
#include "World.h"
#include "RenderDebugger.h"
#include "Entity.h"
#include "Components.h"
#include "Water.h"
#include "logger.h"

VISM2::~VISM2()
{
}

void VISM2::Create(World* world)
{
    this->world = world;
    world->LoadRenderConfig("../3Dprog22/VISM2RenderConfig.json");
    world->GetRenderCamera()->SetCameraPosition(glm::vec3(0.f, 400.f, 0.f));
    world->GetRenderCamera()->SetFar(3000.f);
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
    if (ImGui::Button("Spawn Ball at camera(Can Press R too)"))
    {
        auto* cam = world->GetRenderCamera();
        SpawnBall(cam->GetPosition() + cam->GetDirection() * 10.f);
    }
    ImGui::Text("Task 3.2 - Spawn Balls");
    ImGui::Checkbox("Cast Shadow on rain", &bCastShadowOnRain);
    ImGui::Checkbox("Shaded rain", &bRainShaded);

    if (bRainShaded)
        world->physicsSystem->SetShaderModelOnPhysicsBalls(0);
    else
        world->physicsSystem->SetShaderModelOnPhysicsBalls(6);

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
    
    if (ImGui::Button("Clear Balls"))
    {
        ClearBalls();
    }

    ImGui::Checkbox("Draw Octree", &bDrawOctreeLeafs);
    ImGui::Checkbox("Draw bounding boxes", &bDrawBoundingBoxes);
    ImGui::Checkbox("Draw Convex Hulls", &bDrawConvexHulls);

    ImGui::Checkbox("Enable Multithreading(32 threads)", &world->physicsSystem->bSimulateThreaded);

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    
    
    static float axis[3] { 1250, 1250, 1250};
    static bool changeDir;
    if (ImGui::Button("Set Camera Postion"))
    {
        glm::vec3 newPos = glm::vec3{ axis[0], axis[1], axis[2] };
        world->GetRenderCamera()->SetCameraPosition(newPos);
        if (changeDir == true)
        {
            glm::vec3 newDir = glm::normalize(glm::vec3{ 0.f } - newPos);
            Logger::getInstance()->logText("\nCalculated dir : (" + std::to_string(newDir.x) + ", " + std::to_string(newDir.y) + ", " + std::to_string(newDir.z) + ")", LogType::HIGHLIGHT);
            //float pitch = asin(newDir.y);
            //float yaw = asin(newDir.x / (cos(pitch)+FLT_EPSILON));
            //world->GetRenderCamera()->SetCameraDirection(yaw, pitch);

            glm::vec3 oldDir = world->GetRenderCamera()->GetDirection();
            Logger::getInstance()->logText("Current Dir: (" + std::to_string(oldDir.x) + ", " + std::to_string(oldDir.y) + ", " + std::to_string(oldDir.z) + ")", LogType::HIGHLIGHT);
        }
    }
    ImGui::SameLine();
    ImGui::InputFloat3("(x,y,z)", &axis[0]);
    ImGui::Checkbox("Look towards center on position update?", &changeDir);

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
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere.obj", sphereMesh);
        sphereBody.SetMass(1000.f);    
        sphereBody.SetFriction(0.5f);
    }
    else
    {
        e.AddComponent<PhysicsBall>();
        sphereBody.SetMass(5.f);
        sphereBody.SetElasticity(0.2f);
        sphereBody.SetFriction(0.5f);
    }
    

    collider.SetExtents(glm::vec3(5.2f)); 
   
    e.SetPosition(pos);
    e.SetScale({ 5.f, 5.f, 5.f }); 
}

void VISM2::SpawnBalls(bool bAddSpline)
{
    int height = 3000;
    int iterations = 1;
    int nrRows = numBallsX;
    int nrColumns = numBallsZ;
    float spacing = ballSpacing;
    for (auto f = 0; f < iterations; f++)
        for (auto i = 0; i < nrRows; i++)
        {
            for (auto j = 0; j < nrColumns; j++)
            {
                int randomX = rand() % nrRows;
                int randomY = rand() % height;
                int randomZ = rand() % nrColumns;

                SpawnBall({ randomX * spacing - (nrRows * spacing * 0.5f), 1200.f + randomY, randomZ * spacing - (nrColumns * spacing * 0.5f) }, false, bAddSpline);
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


