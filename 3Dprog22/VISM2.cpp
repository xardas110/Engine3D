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
}


