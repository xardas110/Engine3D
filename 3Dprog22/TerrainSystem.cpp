#include "TerrainSystem.h"
#include "World.h"
#include "Entity.h"
#include "RenderEngine.h"
#include "Components.h"

void TerrainSystem::Init(World* world, entt::registry& registry)
{
    auto group = registry.group<TerrainComponent, TransformComponent>();
    for (auto entity : group)
    {
        auto [terrainComp, transformComp] = group.get<TerrainComponent, TransformComponent>(entity);
        auto& terrain = terrainComp.terrain;

        transformComp.SetPosition({ -terrain.config.scaleXZ * 0.5f, 0.f, -terrain.config.scaleXZ * 0.5f });
        transformComp.SetScale({ terrain.config.scaleXZ, terrain.config.scaleY, terrain.config.scaleXZ });
        terrain.Init();
    }

    InitShaders();
    InitShaderBindings();
    InitMesh(world);
    InitComputeShaders(registry);
    InitNormalBuffers(world, registry);
}

void TerrainSystem::InitMesh(World* world)
{
    auto* meshManager = world->GetMeshManager();
    meshPatch = meshManager->GetTerrainPatchMesh();
}

void TerrainSystem::InitShaders()
{
    auto* renderEngine = RenderEngine::Get();

    if (!renderEngine->GetShader(ShaderManager::ShaderType::Terrain, terrainShader))
    {
        printf("Failed to get terrain shader");
    }

    if (!renderEngine->GetShader(ShaderManager::ShaderType::TerrainNormalCompute, normalComputeShader))
    {
        printf("Failed to get terrain normal compute shader");
    }

    if (!renderEngine->GetShader(ShaderManager::ShaderType::TerrainSplatCompute, splatComputeShader))
    {
        printf("Failed to get terrain splat compute shader");
    }   

    if (!renderEngine->GetShader(ShaderManager::ShaderType::TerrainShadowGrid, sgShader))
    {
        printf("Failed to get terrain shadowgrid shader");
    }

    if (!renderEngine->GetShader(ShaderManager::ShaderType::TerrainShadowMap, spShader))
    {
        printf("Failed to get terrain shadowpass shader");
    }
}

void TerrainSystem::InitShaderBindings()
{
    //For terrain shader first
    auto* re = RenderEngine::Get();

    re->BindShader(terrainShader);
   
    tBindings.scaleY = re->GetUniformLocation("scaleY");
    tBindings.bEnableTess = re->GetUniformLocation("bEnableTess");
    tBindings.bShowHeightLines = re->GetUniformLocation("bShowHeightLines");
    tBindings.worldMatrix = re->GetUniformLocation("worldMatrix");
    tBindings.tessellationFactor = re->GetUniformLocation("tessellationFactor");
    tBindings.tessellationSlope = re->GetUniformLocation("tessellationSlope");
    tBindings.tessellationShift = re->GetUniformLocation("tessellationShift");
    tBindings.heightmap = re->GetUniformLocation("heightmap");
    tBindings.normalMap = re->GetUniformLocation("normalmap");
    tBindings.splatmap = re->GetUniformLocation("splatmap");
    tBindings.TBNRange = re->GetUniformLocation("tessellationRange");
    
    for (size_t i = 0; i < 8; i++)
    {
        std::string location = "lod_morph_area[" + std::to_string(i) + "]";
        tBindings.lodMorphArea[i] = re->GetUniformLocation(location);
    }

    tBindings.localMatrix = re->GetUniformLocation("localMatrix");
    tBindings.location = re->GetUniformLocation("location");
    tBindings.index = re->GetUniformLocation("index");
    tBindings.lod = re->GetUniformLocation("lod");
    tBindings.gap = re->GetUniformLocation("gap");

    tBindings.smCascades = re->GetUniformLocation("smCascades");

    for (int i = 0; i < 3; i++)
    {
        std::string location = "materials[" + std::to_string(i) + "].diffusemap";
        tBindings.materialBindings[i].diffusemap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].normalmap";
        tBindings.materialBindings[i].normalmap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].heightmap";
        tBindings.materialBindings[i].heightmap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].aomap";
        tBindings.materialBindings[i].aomap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].specularmap";
        tBindings.materialBindings[i].specularmap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].roughnessmap";
        tBindings.materialBindings[i].roughnessmap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].heightScaling";
        tBindings.materialBindings[i].heightscale = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].horizontalScaling";
        tBindings.materialBindings[i].horizontalScale = re->GetUniformLocation(location);
    } 

    //normal shader bindings
    re->BindShader(normalComputeShader);
    nBindings.heightmap = re->GetUniformLocation("heightmap");
    nBindings.N = re->GetUniformLocation("N");
    nBindings.strength = re->GetUniformLocation("strength");

    //splat shader bindings
    re->BindShader(splatComputeShader);
    sBindings.normalmap = re->GetUniformLocation("normalmap");
    sBindings.N = re->GetUniformLocation("N");

    //shadowgrid bindings
    re->BindShader(sgShader);
    sgb.heightmap = re->GetUniformLocation("heightmap");
    sgb.scaleY = re->GetUniformLocation("scaleY");
    sgb.localMatrix = re->GetUniformLocation("localMatrix");
    sgb.worldMatrix = re->GetUniformLocation("worldMatrix");

    //shadowpass bindings
    re->BindShader(spShader);

    spBindings.scaleY = re->GetUniformLocation("scaleY");
    spBindings.bEnableTess = re->GetUniformLocation("bEnableTess");
    spBindings.worldMatrix = re->GetUniformLocation("worldMatrix");
    spBindings.tessellationFactor = re->GetUniformLocation("tessellationFactor");
    spBindings.tessellationSlope = re->GetUniformLocation("tessellationSlope");
    spBindings.tessellationShift = re->GetUniformLocation("tessellationShift");
    spBindings.heightmap = re->GetUniformLocation("heightmap");
    spBindings.normalMap = re->GetUniformLocation("normalmap");
    spBindings.splatmap = re->GetUniformLocation("splatmap");
    spBindings.TBNRange = re->GetUniformLocation("tessellationRange");

    for (size_t i = 0; i < 8; i++)
    {
        std::string location = "lod_morph_area[" + std::to_string(i) + "]";
        spBindings.lodMorphArea[i] = re->GetUniformLocation(location);
    }

    spBindings.localMatrix = re->GetUniformLocation("localMatrix");
    spBindings.location = re->GetUniformLocation("location");
    spBindings.index = re->GetUniformLocation("index");
    spBindings.lod = re->GetUniformLocation("lod");
    spBindings.gap = re->GetUniformLocation("gap");

    for (int i = 0; i < 3; i++)
    {
        std::string location = "materials[" + std::to_string(i) + "].normalmap";
        spBindings.materialBindings[i].normalmap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].heightmap";
        spBindings.materialBindings[i].heightmap = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].heightScaling";
        spBindings.materialBindings[i].heightscale = re->GetUniformLocation(location);

        location = "materials[" + std::to_string(i) + "].horizontalScaling";
        spBindings.materialBindings[i].horizontalScale = re->GetUniformLocation(location);
    }
}

void TerrainSystem::InitComputeShaders(entt::registry& registry)
{
    auto* renderEngine = RenderEngine::Get();

    Shader normalShader;
    Shader splatShader;

    if (!renderEngine->GetShader(ShaderManager::ShaderType::TerrainNormalCompute, normalShader))
        printf("Failed to load terrain normal compute shader");

    if (!renderEngine->GetShader(ShaderManager::ShaderType::TerrainSplatCompute, splatShader))
        printf("Failed to load terrain splat compute shader");
    
    auto view = registry.view<TerrainComponent>();
    for (auto entity : view)
    {
        auto& terrainComponent = view.get<TerrainComponent>(entity);
        auto& terrain = terrainComponent.terrain;

        renderEngine->BindShader(normalShader);
        renderEngine->SetTexture(terrain.GetHeightmapTexture(), GL_TEXTURE_2D, nBindings.heightmap, 0);

        renderEngine->SetInt(nBindings.N, terrain.config.N);
        renderEngine->SetFloat(nBindings.strength, terrain.config.strength);
        renderEngine->BindImageTexture(0, terrain.GetNormalmapTexture(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA32F);
        renderEngine->DispatchCompute(terrain.config.N / 16, terrain.config.N / 16, 1);

        renderEngine->BindShader(splatShader);
        renderEngine->SetTexture(terrain.GetNormalmapTexture(), GL_TEXTURE_2D, sBindings.normalmap, 0);

        renderEngine->SetInt(sBindings.N, terrain.config.N);
        renderEngine->BindImageTexture(0, terrain.GetSplatmapTexture(), 0, false, 0, GL_WRITE_ONLY, GL_RGBA16F);
        renderEngine->DispatchCompute(terrain.config.N / 16, terrain.config.N / 16, 1);
    }
}

void TerrainSystem::ShadowPass(World* world, entt::registry& registry)
{
    auto* re = RenderEngine::Get();

    re->SetCulling(GL_BACK);
    re->BindShader(spShader);

    auto group = registry.group<TerrainComponent, TransformComponent>();
    for (auto entity : group)
    {
        auto [terrainComponent, transformComponent] = group.get<TerrainComponent, TransformComponent>(entity);
        auto& terrain = terrainComponent.terrain;

        std::vector<TerrainNode*> terrainNodes;
        terrain.GetNodes(terrainNodes);
  
        re->SetMat4(spBindings.worldMatrix, transformComponent.GetTransform());
        re->SetInt(spBindings.bEnableTess, terrain.config.bEnableTess);
        re->SetFloat(spBindings.scaleY, terrain.config.scaleY);
        re->SetInt(spBindings.TBNRange, terrain.config.tessellationRange);
        re->SetInt(spBindings.tessellationFactor, terrain.config.tessellationFactor);
        re->SetFloat(spBindings.tessellationSlope, terrain.config.tessellationSlope);
        re->SetFloat(spBindings.tessellationShift, terrain.config.tessellationShift);

        re->SetTexture(terrain.GetHeightmapTexture(), GL_TEXTURE_2D, spBindings.heightmap, 0);
        re->SetTexture(terrain.GetNormalmapTexture(), GL_TEXTURE_2D, spBindings.normalMap, 1);
        re->SetTexture(terrain.GetSplatmapTexture(), GL_TEXTURE_2D, spBindings.splatmap, 2);

        for (size_t i = 0; i < 8; i++)
        {
            std::string location = "lod_morph_area[" + std::to_string(i) + "]";
            re->SetInt(spBindings.lodMorphArea[i], terrain.config.lod_morphin_area[i]);
        }

        int texUnit = 3;

        for (int i = 0; i < 3; i++)
        {
            re->SetTexture(terrain.GetMaterial(i).normalmap, GL_TEXTURE_2D, spBindings.materialBindings[i].normalmap, texUnit);
            texUnit++;

            re->SetTexture(terrain.GetMaterial(i).displacementmap, GL_TEXTURE_2D, spBindings.materialBindings[i].heightmap, texUnit);
            texUnit++;

            re->SetFloat(spBindings.materialBindings[i].heightscale, terrain.GetMaterial(i).displaceScale);
            re->SetFloat(spBindings.materialBindings[i].horizontalScale, terrain.GetMaterial(i).horizontalScale);
        }

        for (int i = 0; i < terrainNodes.size(); i++)
        {
            auto* node = terrainNodes[i];

            re->SetMat4(spBindings.localMatrix, node->GetLocalTransform().GetTransform());
            re->SetVec2(spBindings.location, node->GetLocation());
            re->SetVec2(spBindings.index, node->Getindex());
            re->SetInt(spBindings.lod, node->GetLod());
            re->SetFloat(spBindings.gap, node->GetGap());

            re->DrawArrays(GL_PATCHES, meshPatch.vao, 0, 16);
        }
    }
}

void TerrainSystem::Update(World* world, entt::registry& registry)
{
    auto view = registry.view<TerrainComponent>();
    for (auto entity : view)
    {
        auto& terrain = view.get<TerrainComponent>(entity).terrain;
        terrain.Update();
    }
}

void TerrainSystem::Render(World* world, entt::registry& registry)
{
    auto* renderEngine = RenderEngine::Get();

    renderEngine->SetCulling(GL_FRONT);
    renderEngine->EnableDepthTest();
    renderEngine->SetDepthMask(GL_TRUE);
    //will only support 1 dirlight
    DirLight dirlight;
    world->GetDirectionalLight(dirlight);
    renderEngine->EnableWireframe();
    auto group = registry.group<TerrainComponent, TransformComponent>();
    for (auto entity : group)
    {
        auto [terrainComponent, transformComponent] = group.get<TerrainComponent, TransformComponent>(entity);
        auto& terrain = terrainComponent.terrain;

        std::vector<TerrainNode*> terrainNodes;
        terrain.GetNodes(terrainNodes);

        renderEngine->BindShader(terrainShader);
        renderEngine->SetMat4(tBindings.worldMatrix, transformComponent.GetTransform());
        renderEngine->SetInt(tBindings.bEnableTess, terrain.config.bEnableTess);
        renderEngine->SetInt(tBindings.bShowHeightLines, terrain.config.bShowHeightLines);
        renderEngine->SetFloat(tBindings.scaleY, terrain.config.scaleY);
        renderEngine->SetInt(tBindings.TBNRange, terrain.config.tessellationRange);
        renderEngine->SetInt(tBindings.tessellationFactor, terrain.config.tessellationFactor);
        renderEngine->SetFloat(tBindings.tessellationSlope, terrain.config.tessellationSlope);
        renderEngine->SetFloat(tBindings.tessellationShift, terrain.config.tessellationShift);
        renderEngine->SetTexture(dirlight.csm.FBO.texture, GL_TEXTURE_2D_ARRAY, tBindings.smCascades, 31);

        renderEngine->SetTexture(terrain.GetHeightmapTexture(), GL_TEXTURE_2D, tBindings.heightmap, 0);
        renderEngine->SetTexture(terrain.GetNormalmapTexture(), GL_TEXTURE_2D, tBindings.normalMap, 1);
        renderEngine->SetTexture(terrain.GetSplatmapTexture(), GL_TEXTURE_2D, tBindings.splatmap, 2);

        for (size_t i = 0; i < 8; i++)
        {
            std::string location = "lod_morph_area[" + std::to_string(i) + "]";
            renderEngine->SetInt(tBindings.lodMorphArea[i], terrain.config.lod_morphin_area[i]);
        }

        int texUnit = 3;

        for (int i = 0; i < 3; i++)
        {
            renderEngine->SetTexture(terrain.GetMaterial(i).diffusemap, GL_TEXTURE_2D, tBindings.materialBindings[i].diffusemap, texUnit);
            texUnit++;

            renderEngine->SetTexture(terrain.GetMaterial(i).normalmap, GL_TEXTURE_2D, tBindings.materialBindings[i].normalmap, texUnit);
            texUnit++;

            renderEngine->SetTexture(terrain.GetMaterial(i).displacementmap, GL_TEXTURE_2D, tBindings.materialBindings[i].heightmap, texUnit);
            texUnit++;

            renderEngine->SetTexture(terrain.GetMaterial(i).ambientmap, GL_TEXTURE_2D, tBindings.materialBindings[i].aomap, texUnit);
            texUnit++;

            renderEngine->SetTexture(terrain.GetMaterial(i).specularmap, GL_TEXTURE_2D, tBindings.materialBindings[i].specularmap, texUnit);
            texUnit++;

            renderEngine->SetTexture(terrain.GetMaterial(i).roughnessmap, GL_TEXTURE_2D, tBindings.materialBindings[i].roughnessmap, texUnit);
            texUnit++;

            renderEngine->SetFloat(tBindings.materialBindings[i].heightscale, terrain.GetMaterial(i).displaceScale);
            renderEngine->SetFloat(tBindings.materialBindings[i].horizontalScale, terrain.GetMaterial(i).horizontalScale);
        }

        for (int i = 0; i < terrainNodes.size(); i++)
        {
            auto* node = terrainNodes[i];
           
            renderEngine->SetMat4(tBindings.localMatrix, node->GetLocalTransform().GetTransform());
            renderEngine->SetVec2(tBindings.location, node->GetLocation());
            renderEngine->SetVec2(tBindings.index, node->Getindex());
            renderEngine->SetInt(tBindings.lod, node->GetLod());
            renderEngine->SetFloat(tBindings.gap, node->GetGap());
              
            renderEngine->DrawArrays(GL_PATCHES, meshPatch.vao, 0, 16);
        }
    }
    renderEngine->DisableWireframe();
}

void TerrainSystem::InitNormalBuffers(World* world, entt::registry& registry)
{
    auto* textureManager = world->GetTextureManager();

    auto view = registry.view<TerrainComponent>();
    for (auto entity : view)
    {
        auto &terrainComp = view.get<TerrainComponent>(entity);
        auto& terrain = terrainComp.terrain;

        if (!textureManager->LoadSlopeBuffer(terrain.GetNormalmapTexture(), terrain.slopeBuffer.data()))
        {
            std::cout << "failed to load terrain normal texture " << std::endl;
        }
        terrain.normalBuffer.resize(terrain.config.N * terrain.config.N);   
        if (!textureManager->LoadNormalBuffer(terrain.GetNormalmapTexture(), &terrain.normalBuffer[0][0]))
        {
            std::cout << "failed to load terrain normal texture " << std::endl;
        }
    }
}

entt::entity TerrainSystem::CreateTerrain(World* world, entt::registry& registry, const std::string& configJsonPath)
{
    //Leser inn et terreng
    auto* re = RenderEngine::Get();
    auto textureManager = world->GetTextureManager();

    //Some code initialization is temp, maybe later all terrains will get loaded via config files
    auto terrainEnt = world->CreateEntity("Terrain");
    auto& terrain = terrainEnt.AddComponent<TerrainComponent>().terrain;

    terrain.LoadConfig(configJsonPath);

    auto filter = terrain.config.bEnableTess ? GL_LINEAR : GL_NEAREST;

    if (!textureManager->LoadTextureTerrain(terrain.config.heightmapPath, terrain.GetHeightmapTexture(), Texture::Types::Height, filter, filter))
    {
        std::cout << "failed to load terrain heightmap" << std::endl;
    }

    auto width = terrain.GetHeightmapTexture().width;
    auto height = terrain.GetHeightmapTexture().height;

    terrain.config.N = width;

    terrain.heightmapBuffer.resize(width * height);
    terrain.slopeBuffer.resize(width * height);

    if (!textureManager->LoadHeightmapBuffer(terrain.GetHeightmapTexture(), terrain.heightmapBuffer.data()))
    {
        std::cout << "failed to load heightmap buffer" << std::endl;
    }

    textureManager->GenerateTexture("TerrainNormalmap", terrain.GetNormalmapTexture());
    re->Bind2DTexture(terrain.GetNormalmapTexture());
    if (!textureManager->CreateTextureStorage(terrain.GetNormalmapTexture(), width, height, Texture::Types::Normals, GL_LINEAR, GL_LINEAR))
    {
        std::cout << "failed to load terrain normalmap" << std::endl;
    }

    textureManager->GenerateTexture("TerrainSplatmap", terrain.GetSplatmapTexture());
    re->Bind2DTexture(terrain.GetSplatmapTexture());
    if (!textureManager->CreateTextureStorage(terrain.GetSplatmapTexture(), width, height, Texture::Types::Splat, GL_LINEAR, GL_LINEAR, GL_RGBA16F))
    {
        std::cout << "failed to load TerrainSplatmap" << std::endl;
    }

    for (size_t i = 0; i < 3; i++)
    {
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].diffusePath, terrain.GetMaterial(i).diffusemap, Texture::Types::Diffuse, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].normalPath, terrain.GetMaterial(i).normalmap, Texture::Types::Normals, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].displacementPath, terrain.GetMaterial(i).displacementmap, Texture::Types::Displacement, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].aoPath, terrain.GetMaterial(i).ambientmap, Texture::Types::Ambient, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].specularPath, terrain.GetMaterial(i).specularmap, Texture::Types::Specular, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].roughPath, terrain.GetMaterial(i).roughnessmap, Texture::Types::Roughness, 9987, 9729, false);

        terrain.GetMaterial(i).displaceScale = terrain.config.terrainMaterialConfig[i].heightScaling;
        terrain.GetMaterial(i).horizontalScale = terrain.config.terrainMaterialConfig[i].horizontalScaling;
    }

    Init(world, registry);
    return terrainEnt.GetEntityId();
}

entt::entity TerrainSystem::CreateTerrain(World* world, entt::registry& registry, Texture& heightmap, const std::string& configJsonPath)
{
    //Leser inn et terreng
    auto* re = RenderEngine::Get();
    auto textureManager = world->GetTextureManager();

    //Some code initialization is temp, maybe later all terrains will get loaded via config files
    auto terrainEnt = world->CreateEntity("Terrain");
    auto& terrain = terrainEnt.AddComponent<TerrainComponent>().terrain;

    terrain.LoadConfig(configJsonPath);

    terrain.SetHeightmap(heightmap);

    auto width = terrain.GetHeightmapTexture().width;
    auto height = terrain.GetHeightmapTexture().height;

    terrain.config.N = width;

    terrain.heightmapBuffer.resize(width * height);
    terrain.slopeBuffer.resize(width * height);

    if (!textureManager->LoadHeightmapBuffer(terrain.GetHeightmapTexture(), terrain.heightmapBuffer.data()))
    {
        std::cout << "failed to load heightmap buffer" << std::endl;
    }

    textureManager->GenerateTexture("TerrainNormalmap", terrain.GetNormalmapTexture());
    re->Bind2DTexture(terrain.GetNormalmapTexture());
    if (!textureManager->CreateTextureStorage(terrain.GetNormalmapTexture(), width, height, Texture::Types::Normals, GL_LINEAR, GL_LINEAR))
    {
        std::cout << "failed to load terrain normalmap" << std::endl;
    }

    textureManager->GenerateTexture("TerrainSplatmap", terrain.GetSplatmapTexture());
    re->Bind2DTexture(terrain.GetSplatmapTexture());
    if (!textureManager->CreateTextureStorage(terrain.GetSplatmapTexture(), width, height, Texture::Types::Splat, GL_LINEAR, GL_LINEAR, GL_RGBA16F))
    {
        std::cout << "failed to load TerrainSplatmap" << std::endl;
    }

    for (size_t i = 0; i < 3; i++)
    {
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].diffusePath, terrain.GetMaterial(i).diffusemap, Texture::Types::Diffuse, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].normalPath, terrain.GetMaterial(i).normalmap, Texture::Types::Normals, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].displacementPath, terrain.GetMaterial(i).displacementmap, Texture::Types::Displacement, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].aoPath, terrain.GetMaterial(i).ambientmap, Texture::Types::Ambient, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].specularPath, terrain.GetMaterial(i).specularmap, Texture::Types::Specular, 9987, 9729, false);
        textureManager->LoadTexture(terrain.config.terrainMaterialConfig[i].roughPath, terrain.GetMaterial(i).roughnessmap, Texture::Types::Roughness, 9987, 9729, false);

        terrain.GetMaterial(i).displaceScale = terrain.config.terrainMaterialConfig[i].heightScaling;
        terrain.GetMaterial(i).horizontalScale = terrain.config.terrainMaterialConfig[i].horizontalScaling;
    }

    Init(world, registry);
    return terrainEnt.GetEntityId();
}
