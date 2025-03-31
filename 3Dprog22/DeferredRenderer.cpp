#include "DeferredRenderer.h"
#include "RenderEngine.h"
#include "World.h"
#include "renderwindow.h"
#include "RenderDebugger.h"
#include "Include/ImGui/imgui.h"
#include "Include/ImGui/QtImGui.h"

void DeferredRenderer::Init(World* world)
{
    initializeOpenGLFunctions();
    InitQuadMesh(world);
    InitRenderSettingsUBO(world);

    gBufferSystem.Init();
    volumetricLightSystem.Init();
    volumetricClouds.Init(world);
    ssaoSystem.Init();
    bilateralSystem.Init(world);
    smaaSystem.Init(world);
    blurSystem.Init();
    lightPassSystem.Init();
    hdrSystem.Init();  
    gaussianBlurSystem.Init();
    bloomSystem.Init();
    dofSystem.Init();
    perlinNoise.Init(world);
    waterSystem.Init(world, world->entRegistry);
    //wbSystem.Init();

    oitSystem.Init();
    oitSystem.BindShaderToBlock(world->vegetationSystem.mainPass);

    voxelConeTracing.Init(world);
}

void DeferredRenderer::InitRenderSettingsUBO(World* world)
{
    auto* re = RenderEngine::Get();
    auto* uboManager = re->GetUBOManager();
    renderSettingsUBO = uboManager->CreateUBO("renderSettingsUBO", sizeof(RenderSettings), 4);
}

void DeferredRenderer::InitQuadMesh(World* world)
{
    /*TODO: This class should not use the world's meshmanager*/
    auto* meshManager = world->GetMeshManager();
    quadMesh = meshManager->GetQuadMesh();
    unitQuadMesh = meshManager->GetUnitQuad();
}

void DeferredRenderer::UpdateRenderSettingsUBO()
{
    auto* re = RenderEngine::Get();
    re->BindUBO(renderSettingsUBO);

    renderSettings.fogEnabled = config.fog.enabled;
    renderSettings.fogSight = config.fog.sight;
    renderSettings.fogColor = lightPassSystem.fog.fogColor;

    renderSettings.volumetricLightEnabled = config.volumetricLight.enabled;
    renderSettings.giEnabled = config.gi.enabled;

    renderSettings.bloomEnabled = 1;
    renderSettings.bloomThreshold = config.bloom.threshold;

    renderSettings.lightModel = config.lightModel;
    renderSettings.ssaoEnabled = config.ssao.enabled;

    re->BindUBOData(0, sizeof(RenderSettings), &renderSettings);
}

void DeferredRenderer::Update(World* world, float deltatime)
{
    UpdateRenderSettingsUBO();
    voxelConeTracing.Update(world, deltatime);
}

void DeferredRenderer::Render(World* world, float deltatime)
{
    auto* re = RenderEngine::Get();
    Update(world, deltatime);

    if (config.vct.bDisplay)
    {
        ShadowPass(world);
        VoxelPass(world);
        voxelConeTracing.RenderVoxelStructure(quadMesh);
        return;
    }

    //wbSystem.Clear();

    ShadowPass(world);
    VoxelPass(world);

    gBufferSystem.BindGBuffer();
    gBufferSystem.ClearGBuffer();
    oitSystem.Clear();

    MainPass(world);
    AlphaPass(world);
    gBufferSystem.UnbindGBuffer();
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    PostProcessPass(world);
    IndirectLightPass(world);
    FilteringPass(world); 
    LightPass(world);
    BloomPass();
    DoFPass(world);
  
    re->BindFrameBuffer(0);//hdr doesnt own standard framebuffer
    re->GLClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);   
    HDRPass(world);
  
   // BlitFrameBuffer();
    //ForwardPass(world, deltatime);
    //Clear();
    //re->EnableDepthTest();
}

void DeferredRenderer::ShadowPass(World* world)
{
    auto* re = RenderEngine::Get();
    auto* rw = RenderWindow::Get();

    DirLight dirlight;
    world->GetDirectionalLight(dirlight);

    auto& CSM = dirlight.csm;

    re->BindFrameBuffer(CSM.FBO);
    re->SetViewport(0, 0, CSM.cascadeResolution, CSM.cascadeResolution);

    re->EnableDepthTest();
    re->SetDepthFunc(GL_LESS);

    re->EnableCulling();
    re->SetCullFace(GL_BACK);

    re->ClearDepthBuffer();

    world->staticMeshSystem.ShadowPass(world, world->entRegistry);
    world->skeletalMeshSystem.ShadowPass(world, world->entRegistry);
    world->staticMeshInstancedSystem.ShadowPass(world, world->entRegistry);
    world->vegetationSystem.ShadowPass(world, world->entRegistry);
    world->terrainSystem.ShadowPass(world, world->entRegistry);

    re->SetCullFace(GL_FRONT);
    re->DisableCulling();
    re->BindFrameBuffer(0);

    re->SetViewport(
        0,
        0,
        rw->width() * rw->devicePixelRatio(),
        rw->height() * rw->devicePixelRatio()
    );
}

void DeferredRenderer::VoxelPass(World* world)
{
    auto timer = world->GetElapsedTimeInSeconds();

    if ((int)timer % 10000 != 0) return;

    voxelConeTracing.ClearVoxel();
    voxelConeTracing.BindVoxelStructure();
    world->staticMeshSystem.Voxelize(world, world->entRegistry, &voxelConeTracing.voxelStructure);   

    glFinish();
    voxelConeTracing.InjectDirectLight();
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
   // glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    glFinish();
    voxelConeTracing.UnbindVoxelStructure();
    voxelConeTracing.GenerateMipMap();
    glFinish();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    voxelConeTracing.InjectFirstBounce();
    glFinish();
}

void DeferredRenderer::MainPass(World* world)
{
    auto* rd = RenderDebugger::Get();
    auto* re = RenderEngine::Get();

    world->staticMeshSystem.MainPass(world, world->entRegistry);
    world->skeletalMeshSystem.MainPass(world, world->entRegistry);
    world->terrainSystem.Render(world, world->entRegistry);  
    
    world->staticMeshInstancedSystem.MainPass(
        world, 
        world->entRegistry, 
        &oitSystem.aBuffer, 
        world->skySystem.dirlight.csm.FBO.texture, 
        volumetricClouds.cloudShadowVL);
   
    world->vegetationSystem.MainPass(world,
        world->entRegistry,
        nullptr,
        world->skySystem.dirlight.csm.FBO.texture,
        volumetricClouds.cloudShadowVL,
        gBufferSystem.gBuffer.fbo.texture);

    world->cameraSystem.Render(world, world->entRegistry);
    waterSystem.Render(world, world->entRegistry);
    world->skySystem.Render(world, world->entRegistry, config, perlinNoise, &oitSystem.aBuffer);
    rd->Render(World::GetDeltaTime());
    world->bSplineSystem.Render();
}

void DeferredRenderer::AlphaPass(World* world)
{
    world->particleSystem.Render(world, world->entRegistry, &oitSystem.aBuffer);
    world->billboardSystem.Render(world, world->entRegistry, &oitSystem.aBuffer, unitQuadMesh);
    world->staticMeshInstancedSystem.AlphaPass(world, world->skySystem.dirlight.csm.FBO.texture, &oitSystem.aBuffer);
    world->alphaPassSystem.AlphaPass(world, world->entRegistry); 
}

void DeferredRenderer::PostProcessPass(World* world)
{
    volumetricClouds.Process(
        world,
        gBufferSystem.gBuffer,
        quadMesh,
        config.volumetricClouds);

    
    if (config.volumetricLight.enabled)
        volumetricLightSystem.Process(
            world, 
            gBufferSystem.gBuffer.fbo.texture, 
            quadMesh, 
            config,
            &oitSystem.aBuffer);
    
    
    if (config.ssao.enabled)
        ssaoSystem.Process(
            gBufferSystem.gBuffer, 
            quadMesh, 
            config);
}

void DeferredRenderer::FilteringPass(World* world)
{
    bilateralSystem.Filter(
        volumetricLightSystem.volumetricLight.fbo.texture, volumetricLightSystem.volumetricLight.filteredTexture, quadMesh);
        
    smaaSystem.Filter(
        volumetricLightSystem.volumetricLight.filteredTexture, volumetricLightSystem.volumetricLight.upScaledTexture, quadMesh);   

    if (volumetricClouds.IsDownscaled())
    { 
        bilateralSystem.Filter(
            volumetricClouds.cloudColor,
            volumetricClouds.cloudColorBlur,
            quadMesh);
            
        smaaSystem.Filter(
            volumetricClouds.cloudColorBlur,
            volumetricClouds.cloudColorUpscaled,
            quadMesh);           
    }

    
    if (config.ssao.enabled)
        blurSystem.Filter(
            ssaoSystem.ssao.fbo.texture,
            ssaoSystem.ssao.upscaledColor,
            quadMesh);


    bilateralSystem.Filter(
       voxelConeTracing.vct.il.diffuseAO, 
        voxelConeTracing.vct.il.diffuseAOFiltered,
        quadMesh);
    
    /*
    smaaSystem.Filter(
        ssaoSystem.ssao.blur,
        ssaoSystem.ssao.upscaledColor,
        quadMesh);
     */   
}

void DeferredRenderer::IndirectLightPass(World* world)
{
    if (!config.gi.enabled) return;
    voxelConeTracing.TraceCones(gBufferSystem.gBuffer, quadMesh);
}

void DeferredRenderer::LightPass(World* world)
{
    lightPassSystem.Render(
        world, 
        gBufferSystem.gBuffer, 
        &oitSystem.aBuffer,
        volumetricLightSystem.volumetricLight.upScaledTexture,
        ssaoSystem.ssao.upscaledColor,
        quadMesh, 
        voxelConeTracing.vct.il,
        volumetricClouds);
}

void DeferredRenderer::BloomPass()
{
    if (!config.bloom.enabled) return;

    gaussianBlurSystem.Filter(
        lightPassSystem.lp.brightColor, 
        bloomSystem.bloom.blur, 
        quadMesh);

    smaaSystem.Filter(
        bloomSystem.bloom.blur, 
        bloomSystem.bloom.blurUpscaled, 
        quadMesh);
}

void DeferredRenderer::DoFPass(World* world)
{
    if (config.dof.enabled)
        dofSystem.Process(
            world,
            lightPassSystem.lp.color,
            gBufferSystem.gBuffer.fbo.texture,
            quadMesh, config);    
}

void DeferredRenderer::HDRPass(World* world)
{
    if (config.dof.enabled)
        hdrSystem.Render(dofSystem.dof.blur, bloomSystem.bloom.blurUpscaled, quadMesh, config);
    else
        hdrSystem.Render(lightPassSystem.lp.color, bloomSystem.bloom.blurUpscaled, quadMesh, config);
}

void DeferredRenderer::BlitFrameBuffer()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferSystem.gBuffer.fbo.fboId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, GetWindowWidth(), GetWindowHeight(), 0, 0, GetWindowWidth(), GetWindowHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::ForwardPass(World* world, float deltatime)
{
    /*
    auto* rd = RenderDebugger::Get();
    auto* re = RenderEngine::Get();
    rd->Render(deltatime);
    */
}

void DeferredRenderer::OnResize()
{
    auto width = GetWindowWidth();
    auto height = GetWindowHeight();

    gBufferSystem.OnResize(width, height);
    volumetricLightSystem.OnResize(width, height);
    lightPassSystem.OnResize(width, height);
    ssaoSystem.OnResize(width, height);
    bloomSystem.OnResize(width, height);
    dofSystem.OnResize(width, height);
    volumetricClouds.OnResize(width, height);
    oitSystem.OnResize(width, height);
    voxelConeTracing.OnResize(width, height);
}

void DeferredRenderer::CleanRenderSettingsUBO()
{
    auto* re = RenderEngine::Get();
    auto uboManager = re->GetUBOManager();
    uboManager->DeleteUBO("RenderSettingsUBO");
}

void DeferredRenderer::Clear()
{
    lightPassSystem.OnClear();  
}

void DeferredRenderer::Clean()
{
    volumetricLightSystem.Clean();
    bilateralSystem.Clean();
    smaaSystem.Clean();
    gBufferSystem.Clean();
    hdrSystem.Clean();
    lightPassSystem.Clean();
    ssaoSystem.Clean();
    blurSystem.Clean();
    gaussianBlurSystem.Clean();
    bloomSystem.Clean();
    dofSystem.Clean();
    oitSystem.Clean();
    volumetricClouds.Clean();
    voxelConeTracing.Clean();
    CleanRenderSettingsUBO();
   // wbSystem.Clean();
}

const DeferredRendererConfig& DeferredRenderer::LoadConfig(const std::string& path)
{
    config.Load(path);
    return config;
}

int DeferredRenderer::GetWindowWidth()
{
    auto* renderWindow = RenderWindow::Get();
    return renderWindow->GetWidth();
}

int DeferredRenderer::GetWindowHeight()
{
    auto* renderWindow = RenderWindow::Get();
    return renderWindow->GetHeight();
}

void DeferredRenderer::ReloadVoxels(World *world)
{
    voxelConeTracing.ClearVoxel();
    voxelConeTracing.BindVoxelStructure();
    world->staticMeshSystem.Voxelize(world, world->entRegistry, &voxelConeTracing.voxelStructure);

    glFinish();
    voxelConeTracing.InjectDirectLight();
    //glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    // glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    glFinish();
    voxelConeTracing.UnbindVoxelStructure();
    voxelConeTracing.GenerateMipMap();
    glFinish();
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    voxelConeTracing.InjectFirstBounce();
    glFinish();
}

void DeferredRenderer::LoadRenderConfig(const std::string& jsonPath)
{
    config.Load(jsonPath);
}
