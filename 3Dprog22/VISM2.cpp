#include "VISM2.h"
#include "World.h"
#include "RenderDebugger.h"
#include "Entity.h"
#include "Components.h"

VISM2::~VISM2()
{
}

void VISM2::Create(World* world)
{
    this->world = world;
    world->LoadRenderConfig("../3Dprog22/VISM2RenderConfig.json");
    world->GetRenderCamera()->SetCameraPosition(glm::vec3(0.f, 400.f, 0.f));
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
    if (ImGui::Button("Spawn Balls"))
    {
        ClearBalls();
        SpawnBalls();
    }
    ImGui::SliderInt("Num Balls X", &numBallsX, 10, 100);
    ImGui::SliderInt("Num Balls Y", &numBallsZ, 10, 100);
    ImGui::SliderInt("Ball Spacing", &ballSpacing, 10, 100);
    ImGui::Checkbox("Enable Multithreading(32 threads)", &world->physicsSystem->bSimulateThreaded);

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

void VISM2::SpawnBall(const glm::vec3& pos, bool bHighres)
{
    auto sm = world->GetStaticMeshManager();

    Entity e = world->CreateEntity("Ball" + std::to_string(nextNameId++));
    balls.emplace_back(e.GetEntityId());

    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::Sphere).col;
    auto& sphereBody = e.AddComponent<PhysicsComponent>().body;

    if (bHighres)
    { 
        auto& sphereMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere_LOD4.obj", sphereMesh);
    }
    else
    {
        auto& sphereBody = e.AddComponent<PhysicsBall>();
       // sphereMesh.AddMesh(world->GetMeshManager()->GetOctahedronBallMesh());
       // sphereMesh.SetColor(glm::vec3(0.1f, 0.1f, 0.7f));
    }
    sphereBody.SetMass(1000.f);
    sphereBody.SetFriction(0.5f);

    collider.SetExtents(glm::vec3(4.8f, 4.8f, 4.8f)); 
   
    e.SetPosition(pos);
    e.SetScale({ 5.f, 5.f, 5.f }); 
}

void VISM2::SpawnBalls()
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
                SpawnBall({ i * spacing - (nrRows * spacing * 0.5f), 1500.f * (f + 1), j * spacing - (nrColumns * spacing * 0.5f) }, false);
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


