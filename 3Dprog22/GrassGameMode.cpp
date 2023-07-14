#include "GrassGameMode.h"
#include "World.h"
#include "MaterialManager.h"
#include "Components.h"
#include "Entity.h"
#include "Character.h"
#include <random>

GrassGameMode::~GrassGameMode()
{
}

void GrassGameMode::Create(World* world, entt::registry& registry)
{
    CreateTerrain(world);
    CreateScene(world);
    CreateTrees(world);
    world->LoadRenderConfig("../3dprog22/Config/Renderer/ConfigSponza.json");
}

void GrassGameMode::BeginPlay(World* world, entt::registry& registry)
{
}

void GrassGameMode::EndPlay(World* world, entt::registry& registry)
{
}

void GrassGameMode::Update(World* world, entt::registry& registry, float deltatime)
{
    UpdatePlayer(world, deltatime);
}

void GrassGameMode::UpdateEditor(World* world, entt::registry& registry, float deltatime)
{
}

void GrassGameMode::Reset(World* world, entt::registry& registry)
{
}

void GrassGameMode::CreateTerrain(World* world)
{
    entt::entity terrainId = world->CreateTerrain("../3Dprog22/Config/Terrain/Terrain1.json");

    Entity terrainEnt(terrainId, world);

    auto& terrain = terrainEnt.GetComponent<TerrainComponent>().terrain;

    auto& config = terrain.config;
    CollisionHeightmap heightmap;

    glm::vec3 scale(config.scaleXZ, config.scaleY, config.scaleXZ);

    heightmap.SetHeightmap(config.N, config.N, scale, &terrain.heightmapBuffer);

    world->physicsSystem->RegisterHeightmap(heightmap);
}

void GrassGameMode::CreateScene(World* world)
{
    constexpr float mapSize = 2700;

    auto& materialManager = MaterialManager::Get();
    auto staticMeshManager = world->GetStaticMeshManager();

    { // for we are many grass
        auto instancedEnt = world->CreateEntity("Instanced grass");
        auto& veg = instancedEnt.AddComponent<VegetationComponent>().veg;

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/Grass3/Textures/Atlas/vlkhcbxia_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Atlas/vlkhcbxia_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Atlas/vlkhcbxia_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Atlas/vlkhcbxia_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Atlas/vlkhcbxia_2K_Specular.jpg", Texture::Specular},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Atlas/vlkhcbxia_2K_Roughness.jpg", Texture::Roughness},
            },
            veg.atlas);

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/Grass3/Textures/Billboard/Billboard_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Billboard/Billboard_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Billboard/Billboard_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Billboard/Billboard_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Billboard/Billboard_2K_Specular.jpg", Texture::Specular},
                {"../3Dprog22/Assets/Vegetation/Grass3/Textures/Billboard/vlkhcbxia_2K_Roughness.jpg", Texture::Roughness},
            },
            veg.billboard);


        for (size_t i = 0; i < 20; i++)
        {
            Variant variant;

            const std::string varString = "Var" + std::to_string(i + 1);

            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Grass3/" + varString + '/' + varString + "_LOD0.fbx", variant.LOD[0]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Grass3/" + varString + '/' + varString + "_LOD1.fbx", variant.LOD[1]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Grass3/" + varString + '/' + varString + "_LOD2.fbx", variant.LOD[2]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Grass3/" + varString + '/' + varString + "_LOD3.fbx", variant.LOD[3]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Grass3/" + varString + '/' + varString + "_LOD4.fbx", variant.LOD[4]);

            for (size_t i = 0; i < 20000; i++)
            {
                glm::mat4 translation(1.f);
                float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float y = world->GetTerrainHeightAt(x, z);
                float ns = world->GetTerrainSlopeAt(x, z);

                if (ns < 0.4f) continue;

                translation = glm::translate(translation, { x, y, z });

                glm::mat4 scale(1.f);
                scale = glm::scale(scale, { 0.45f, 0.45f, 0.45f });

                glm::mat4 rot(1.f);
                rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));

                glm::mat4 finale = translation * rot * scale;
                variant.transforms.emplace_back(finale);
            }

            veg.variants.emplace_back(variant);
        }
    }

    { // for we are many grass
        auto instancedEnt = world->CreateEntity("Instanced Perennials");
        auto& veg = instancedEnt.AddComponent<VegetationComponent>().veg;

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/Perennials/Textures/Atlas/tecrxcp_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Atlas/tecrxcp_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Atlas/tecrxcp_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Atlas/tecrxcp_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Atlas/tecrxcp_2K_Specular.jpg", Texture::Specular},
            },
            veg.atlas);

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/Perennials/Textures/Billboard/Billboard_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Billboard/Billboard_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Billboard/Billboard_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Billboard/Billboard_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/Perennials/Textures/Billboard/Billboard_2K_Specular.jpg", Texture::Specular},
            },
            veg.billboard);


        for (size_t i = 0; i < 8; i++)
        {
            Variant variant;

            const std::string varString = "Var" + std::to_string(i + 1);

            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Perennials/" + varString + '/' + varString + "_LOD0.fbx", variant.LOD[0]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Perennials/" + varString + '/' + varString + "_LOD1.fbx", variant.LOD[1]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Perennials/" + varString + '/' + varString + "_LOD2.fbx", variant.LOD[2]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Perennials/" + varString + '/' + varString + "_LOD3.fbx", variant.LOD[3]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Perennials/" + varString + '/' + varString + "_LOD4.fbx", variant.LOD[4]);

            for (size_t i = 0; i < 1000; i++)
            {
                glm::mat4 translation(1.f);
                float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float y = world->GetTerrainHeightAt(x, z);
                float ns = world->GetTerrainSlopeAt(x, z);

                if (ns < 0.4f) continue;

                translation = glm::translate(translation, { x, y, z });

                glm::mat4 scale(1.f);

                scale = glm::scale(scale, { 0.15f, 0.15f, 0.15f });

                glm::mat4 rot(1.f);
                rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));

                glm::mat4 finale = translation * rot * scale;
                variant.transforms.emplace_back(finale);
            }

            veg.variants.emplace_back(variant);
        }
    }

    { // for we are many grass
        auto instancedEnt = world->CreateEntity("Instanced Field Poppy 14");
        auto& veg = instancedEnt.AddComponent<VegetationComponent>().veg;

        const std::string typeString = "vmcobd0ja";

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Atlas/" + typeString + "_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Atlas/" + typeString + "_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Atlas/" + typeString + "_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Atlas/" + typeString + "_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Atlas/" + typeString + "_2K_Specular.jpg", Texture::Specular},
            },
            veg.atlas);

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Billboard/Billboard_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Billboard/Billboard_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Billboard/Billboard_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Billboard/Billboard_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/Field Poppy/Textures/Billboard/Billboard_2K_Specular.jpg", Texture::Specular},
            },
            veg.billboard);


        for (size_t i = 0; i < 14; i++)
        {
            Variant variant;

            const std::string varString = "Var" + std::to_string(i + 1);

            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Field Poppy/" + varString + '/' + varString + "_LOD0.fbx", variant.LOD[0]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Field Poppy/" + varString + '/' + varString + "_LOD1.fbx", variant.LOD[1]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Field Poppy/" + varString + '/' + varString + "_LOD2.fbx", variant.LOD[2]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Field Poppy/" + varString + '/' + varString + "_LOD3.fbx", variant.LOD[3]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/Field Poppy/" + varString + '/' + varString + "_LOD4.fbx", variant.LOD[4]);

            for (size_t i = 0; i < 1000; i++)
            {
                glm::mat4 translation(1.f);
                float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float y = world->GetTerrainHeightAt(x, z);
                float ns = world->GetTerrainSlopeAt(x, z);

                if (ns < 0.4f) continue;

                translation = glm::translate(translation, { x, y, z });

                glm::mat4 scale(1.f);

                scale = glm::scale(scale, { 0.1f, 0.1f, 0.1f });

                glm::mat4 rot(1.f);
                rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));

                glm::mat4 finale = translation * rot * scale;
                variant.transforms.emplace_back(finale);
            }

            veg.variants.emplace_back(variant);
        }
    }
    { // for we are many grass
        auto instancedEnt = world->CreateEntity("Instanced annuals 7");
        auto& veg = instancedEnt.AddComponent<VegetationComponent>().veg;

        const std::string name = "Annuals";
        const std::string typeString = "tjlnfikia";

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/" + name + "/Textures/Atlas/" + typeString + "_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Atlas/" + typeString + "_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Atlas/" + typeString + "_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Atlas/" + typeString + "_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Atlas/" + typeString + "_2K_Specular.jpg", Texture::Specular},
            },
            veg.atlas);

        materialManager.LoadInstancedMaterials(
            { { "../3Dprog22/Assets/Vegetation/" + name + "/Textures/Billboard/Billboard_2K_Albedo.jpg", Texture::Diffuse},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Billboard/Billboard_2K_AO.jpg", Texture::Ambient},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Billboard/Billboard_2K_Normal.jpg", Texture::Normals},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Billboard/Billboard_2K_Opacity.jpg", Texture::Opacity},
                {"../3Dprog22/Assets/Vegetation/" + name + "/Textures/Billboard/Billboard_2K_Specular.jpg", Texture::Specular},
            },
            veg.billboard);


        for (size_t i = 0; i < 7; i++)
        {
            Variant variant;

            const std::string varString = "Var" + std::to_string(i + 1);

            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/" + name + "/" + varString + '/' + varString + "_LOD0.fbx", variant.LOD[0]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/" + name + "/" + varString + '/' + varString + "_LOD1.fbx", variant.LOD[1]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/" + name + "/" + varString + '/' + varString + "_LOD2.fbx", variant.LOD[2]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/" + name + "/" + varString + '/' + varString + "_LOD3.fbx", variant.LOD[3]);
            staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Vegetation/" + name + "/" + varString + '/' + varString + "_LOD4.fbx", variant.LOD[4]);

            for (size_t i = 0; i < 1000; i++)
            {
                glm::mat4 translation(1.f);
                float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / mapSize)) - mapSize * 0.5f;
                float y = world->GetTerrainHeightAt(x, z);
                float ns = world->GetTerrainSlopeAt(x, z);

                if (ns < 0.4f) continue;

                translation = glm::translate(translation, { x, y, z });

                glm::mat4 scale(1.f);

                scale = glm::scale(scale, { 0.1f, 0.1f, 0.1f });

                glm::mat4 rot(1.f);
                rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));

                glm::mat4 finale = translation * rot * scale;
                variant.transforms.emplace_back(finale);
            }

            veg.variants.emplace_back(variant);
        }
    }
    {
        Entity player = world->CreateEntity("Player");
        playerEnt = player.GetEntityId();
        player.AddComponent<ScriptComponent>().Bind<Character>();

        glm::vec3 pPos = player.GetPosition();

        float y = world->GetTerrainHeightAt(pPos.x, pPos.z);
        pPos.y = y + 50.f;

        player.SetPosition(pPos);
    }
}

void GrassGameMode::CreateTrees(World* world)
{
    auto mapSize = 2700.f;

    auto sm = world->GetStaticMeshManager();

    std::uniform_real_distribution<float> rf(-1500.f, 1500.f);
    std::default_random_engine engine;

    /*
    {
        Entity tree = world->CreateEntity("TreeInstanced");
        auto& instance = tree.AddComponent<StaticMeshInstancedComponent>().staticMeshInstanced;

        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree1/Lod0.obj", instance.LOD[0]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree1/Lod1.obj", instance.LOD[1]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree1/Lod2.obj", instance.LOD[2]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree1/Lod3.obj", instance.LOD[3]);

        for (size_t i = 0; i < 300; i++)
        {
            float x = rf(engine);
            float z = rf(engine);
            float y = world->GetTerrainHeightAt(x, z);
            float ns = world->GetTerrainSlopeAt(x, z);

            if (ns < 0.4f) continue;

            glm::mat4 model(1.f);
            glm::mat4 translate(1.f);
            translate = glm::translate(translate, { x, y, z });

            glm::mat4 scale(1.f);
            scale = glm::scale(scale, { 20.f, 20.f, 20.f });

            glm::mat4 rot(1.f);
            rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));

            model = translate * rot * scale;

            instance.transforms.emplace_back(model);
        }
    }
    */

    {
        Entity tree = world->CreateEntity("TreeInstanced1");
        auto& instance = tree.AddComponent<StaticMeshInstancedComponent>().staticMeshInstanced;

        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/LOD0.obj", instance.LOD[0], true);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/LOD1.obj", instance.LOD[1]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/LOD2.obj", instance.LOD[2]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/LOD3.obj", instance.LOD[3]);

        auto ch = sm->GetConvexHull("../3Dprog22/Assets/Models/Tree2/LOD0.obj");

        for (size_t i = 0; i < 300; i++)
        {
            float x = rf(engine);
            float z = rf(engine);
            float y = world->GetTerrainHeightAt(x, z);
            float ns = world->GetTerrainSlopeAt(x, z);

            if (ns < 0.4f) continue;

            glm::mat4 model(1.f);
            glm::mat4 translate(1.f);
            translate = glm::translate(translate, { x, y, z });

            glm::mat4 scale(1.f);
            scale = glm::scale(scale, { 20.f, 20.f, 20.f });

            glm::mat4 rot(1.f);
            rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));

            model = translate * rot * scale;

            instance.transforms.emplace_back(model);

            Entity ent = world->CreateEntity("TreeCol" + std::to_string(i));
            auto& body = ent.AddComponent<PhysicsComponent>().body;
            auto& col = ent.AddComponent<CollisionComponent>(CollideableType::ConvexHull).col;
            body.SetMass(0.f);
            col.SetConvexHull(ch);
            ent.SetTransform(model);
            col.SetExtents({ 5.f, 20.f, 5.f });
        }
    }
}

void GrassGameMode::UpdatePlayer(World* world, float deltatime)
{   
    Entity playerEntity(playerEnt, world);
    auto pos = playerEntity.GetPosition();
    pos.y = world->GetTerrainHeightAt(pos.x, pos.z) + 13.f;
    playerEntity.SetPosition(pos);
}
