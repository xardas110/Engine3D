#include "SkySystem.h"
#include "Texture.h"
#include "Mesh.h"
#include "World.h"
#include "RenderEngine.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "HelperMath.h"
#include "Debug.h"
#include "RenderEngine.h"
#include "Include/ImGui/imgui.h"
SkySystem::SkySystem() {}

SkySystem::~SkySystem()
{
   // delete skybox;
   // skybox = nullptr;
}

void SkySystem::Init(World* world, entt::registry& registry)
{
    InitShaders(world);
    InitQuadMesh(world);
    InitShaderBindings();
	//InitSkybox(world);
    InitSkyDome(world);
    InitDirlight(world);
    InitSun(world);
    //InitLensFlares(world);
}

void SkySystem::InitShaders(World* world)
{
    auto* shaderManager = RenderEngine::Get()->GetShaderManager();

    shaderManager->GetShader(ShaderManager::Skybox, skyboxShader);
    shaderManager->GetShader(ShaderManager::SkyDome, skyDome.shader);
    shaderManager->GetShader(ShaderManager::Sun, sunShader);
    shaderManager->GetShader(ShaderManager::LensFlare, lensShader); 
}

void SkySystem::InitShaderBindings()
{
    InitSkyboxShaderBindings();
    InitSunShaderBindings();
    InitLensShaderBindings();
    InitSkyDomeShaderBindings();
}

void SkySystem::InitSkyboxShaderBindings()
{
    auto* renderEngine = RenderEngine::Get();

    renderEngine->BindShader(skyboxShader);
    skyboxBindings.MVP = renderEngine->GetUniformLocation("MVP");
    skyboxBindings.skybox = renderEngine->GetUniformLocation("skybox");
}

void SkySystem::InitSkyDomeShaderBindings()
{
    auto* renderEngine = RenderEngine::Get();

    renderEngine->BindShader(skyDome.shader);
    skyDomeBindings.model =  renderEngine->GetUniformLocation("model");
    skyDomeBindings.skydomeTexture = renderEngine->GetUniformLocation("skydomeTexture");
    skyDomeBindings.botColor = renderEngine->GetUniformLocation("botColor");
    skyDomeBindings.topColor = renderEngine->GetUniformLocation("topColor");
    skyDomeBindings.cloudColor = renderEngine->GetUniformLocation("cloudColor");
    skyDomeBindings.perlinTex = renderEngine->GetUniformLocation("perlinTex");
}

void SkySystem::InitSunShaderBindings()
{
    auto* renderEngine = RenderEngine::Get();

    renderEngine->BindShader(sunShader);
    sunBindings.MVP = renderEngine->GetUniformLocation("MVP");
    sunBindings.texture = renderEngine->GetUniformLocation("texture");
    sunBindings.sunInnerTexture = renderEngine->GetUniformLocation("sunInnerTexture");
}

void SkySystem::InitLensShaderBindings()
{
    auto* renderEngine = RenderEngine::Get();

    renderEngine->BindShader(lensShader);
    lensBindings.lensTexture = renderEngine->GetUniformLocation("lensTexture");
    lensBindings.pos2D = renderEngine->GetUniformLocation("pos2D");
    lensBindings.scale2D = renderEngine->GetUniformLocation("scale2D");
    lensBindings.brightness = renderEngine->GetUniformLocation("brightness");
}

const DirLight& SkySystem::GetDirectionalLight()
{
    return dirlight;
}

void SkySystem::SetLightDirection(const glm::vec3& newDir)
{
    dirlight.lightDir = newDir;
}

void SkySystem::SetLightAmbient(const glm::vec3& newAmbient)
{
    dirlight.ambient = newAmbient;
}

void SkySystem::SetLightDiffuse(const glm::vec3& newDiffuse)
{
    dirlight.diffuse = newDiffuse;
}

void SkySystem::SetLightSpecular(const glm::vec3& newSpecular)
{
    dirlight.specular = newSpecular;
}

void SkySystem::SetLightItensity(const float newVal)
{
    dirlight.itensity = newVal;
}

const glm::vec3& SkySystem::GetLightDirection() const
{
    return dirlight.lightDir;
}

const glm::vec3& SkySystem::GetAmbientLight() const
{
    return dirlight.ambient;
}

const glm::vec3& SkySystem::GetDiffuseLight() const
{
    return dirlight.diffuse;
}

const glm::vec3& SkySystem::GetSpecularLight() const
{
    return dirlight.specular;
}

const float SkySystem::GetItensity() const
{
    return dirlight.itensity;
}

void SkySystem::InitQuadMesh(World* world)
{
    quadMesh = world->GetMeshManager()->GetQuadMesh();
}

void SkySystem::InitSkybox(World* world)
{
    auto* textureManager = world->GetTextureManager();
    auto* meshManager = world->GetMeshManager();

    std::array<std::string, 6> skyboxTextures
    {
        "../3Dprog22/Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_left.png",
        "../3Dprog22/Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_right.png",
        "../3Dprog22/Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_up.png",
        "../3Dprog22/Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_down.png",
        "../3Dprog22/Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_front.png",
        "../3Dprog22/Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_back.png"
    };

    CubemapDescriptor descriptor;
    Texture texture;
    Mesh mesh;

    descriptor.tag = "Skybox";
    descriptor.texturePaths = skyboxTextures;

    if (!textureManager->LoadCubemapTextures(descriptor, texture))
        std::cout << "failed to load skybox textures" << std::endl;

    if (!meshManager->GetSkyboxMesh(mesh))
        std::cout << "failed to load skybox mesh" << std::endl;

    //skybox = new Skybox(mesh, texture);
}

void SkySystem::InitSkyDome(World* world)
{
    auto* staticMeshManager = world->GetStaticMeshManager();
    staticMeshManager->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/SphereUntextured.obj", skyDome.mesh);
}

void SkySystem::InitDirlight(World* world)
{
    auto* renderEngine = RenderEngine::Get();
    auto* fboManager = renderEngine->GetFBOManager();
    auto* uboManager = renderEngine->GetUBOManager();

    auto& csm = dirlight.csm;

    csm.FBO = fboManager->CreateCascadedFBO(
        "DirLightCascades", 
        csm.cascadeResolution, 
        csm.cascadeResolution, 
        MAX_CASCADES);

    if (!csm.FBO.IsFBOValid())
    {
        std::cout << "fbo is invalid " << std::endl;
    }

    csm.UBO = uboManager->CreateUBO(
        "DirLightCSM",
        sizeof(csm.cascades) +
        sizeof(csm.toShadowProjection),
        0);

    if (!csm.UBO.IsValid())
    {
        std::cout << "UBO is invalid " << std::endl;
    }

    dirlight.ubo = uboManager->CreateUBO("DirlightUBO", 64, 1);
}

void SkySystem::InitSun(World* world)
{
    auto* meshManager = world->GetMeshManager();
    auto* texturemanger = world->GetTextureManager();
    auto* camera = world->GetRenderCamera();

    DirLight dirLight;
    world->GetDirectionalLight(dirLight);

    if (!texturemanger->LoadTexture("../3Dprog22/Assets/Textures/Sun/sun.png", sun.texture))
        printf("Failed to load sun texture");

    if (!texturemanger->LoadTexture("../3Dprog22/Assets/Textures/Sun/sun_small.png", sun.innerTexture))
        printf("Failed to load sun inner texture");

    sun.mesh = meshManager->GetQuadMesh();

    auto camPos = camera->GetPosition();
    auto worldUp = World::GetWorldUp();

    auto sunDir = glm::normalize(dirLight.lightDir);
    auto sunRight = glm::cross(worldUp, sunDir);
    auto sunUp = glm::cross(sunDir, sunRight);

    glm::mat4 sunRot =
    {
        glm::vec4(sunRight, 0.f),
        glm::vec4(sunUp, 0.f),
        glm::vec4(sunDir, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f),
    };

    sun.transform.SetPosition(-sunDir);
    sun.transform.SetRotation(sunRot);
    sun.transform.SetScale({ 0.2f, 0.2f, 0 });
}

void SkySystem::InitLensFlares(World* world)
{
    auto* textureManager = world->GetTextureManager();

    Texture texture2, texture3, texture4, texture5, texture6, texture7, texture8;

    if (!textureManager->LoadTexture("../3Dprog22/Assets/Textures/Lens flare/tex2.png", texture2))
        printf("Failed to load lens texture: %i", 2);  

    if (!textureManager->LoadTexture("../3Dprog22/Assets/Textures/Lens flare/tex3.png", texture3))
        printf("Failed to load lens texture: %i", 3);

    if (!textureManager->LoadTexture("../3Dprog22/Assets/Textures/Lens flare/tex4.png", texture4))
        printf("Failed to load lens texture: %i", 4);

    if (!textureManager->LoadTexture("../3Dprog22/Assets/Textures/Lens flare/tex5.png", texture5))
        printf("Failed to load lens texture: %i", 5);

    if (!textureManager->LoadTexture("../3Dprog22/Assets/Textures/Lens flare/tex6.png", texture6))
        printf("Failed to load lens texture: %i", 6);

    if (!textureManager->LoadTexture("../3Dprog22/Assets/Textures/Lens flare/tex7.png", texture7))
        printf("Failed to load lens texture: %i", 7);

    if (!textureManager->LoadTexture("../3Dprog22/Assets/Textures/Lens flare/tex8.png", texture8))
        printf("Failed to load lens texture: %i", 8);

    lensFlares.AddFlare({texture6, 0.5f});

    lensFlares.AddFlare({ texture4, 0.23f });
    lensFlares.AddFlare({ texture2, 0.1f });
    lensFlares.AddFlare({ texture7, 0.05f });
   
    lensFlares.AddFlare({ texture3, 0.06f });
    lensFlares.AddFlare({ texture5, 0.07f });
    lensFlares.AddFlare({ texture7, 0.2f });

    lensFlares.AddFlare({ texture3, 0.07f });
    lensFlares.AddFlare({ texture5, 0.3f });
    lensFlares.AddFlare({ texture7, 0.4f });

    lensFlares.AddFlare({ texture8, 0.6f });
}

void SkySystem::UpdateDirlight(World* world)
{
    auto* renderEngine = RenderEngine::Get();
    auto* camera = world->GetRenderCamera();

    auto frustumCorners = camera->GetFrustumCorners();
    auto camNear = camera->GetNear();
    auto camFar = camera->GetFar();

    auto cascades = dirlight.GetCascades(frustumCorners, camNear, camFar);

    auto& CSM = dirlight.csm;
    auto& cascadeProjectionMatrix = CSM.toShadowProjection;
    
    renderEngine->BindUBO(dirlight.csm.UBO);
    renderEngine->BindUBOData(0, sizeof(cascadeProjectionMatrix), &cascadeProjectionMatrix);
    renderEngine->BindUBOData(sizeof(cascadeProjectionMatrix), sizeof(cascades), &cascades);

    std::vector<glm::vec4> dirlightData;

    dirlightData.emplace_back(glm::vec4(dirlight.ambient, 0.f ));
    dirlightData.emplace_back(glm::vec4(dirlight.diffuse, 0.f));
    dirlightData.emplace_back(glm::vec4(dirlight.specular, 0.f));
    dirlightData.emplace_back(glm::vec4(dirlight.lightDir, dirlight.itensity));

    renderEngine->BindUBO(dirlight.ubo);
    renderEngine->BindUBOData(
        0, 
        dirlightData.size() * sizeof(dirlightData[0]),
        dirlightData.data());
}

void SkySystem::Update(World* world, entt::registry& registry, float deltaTime)
{
    this->deltaTime = deltaTime;

    ImGui::Begin("SkySystem");
    if (ImGui::SliderFloat3("Light Direction", &dirlight.lightDir.x, -1, +1))
    {
        if (dirlight.lightDir == glm::vec3(0.f, 0.f, 0.f))
            dirlight.lightDir = glm::vec3(0.f, -1.f, 0.f);
        else
            dirlight.lightDir = glm::normalize(dirlight.lightDir);
    }
    ImGui::End();
    UpdateDirlight(world);
    UpdateSun(world, deltaTime);
}

void SkySystem::Render(World* world, entt::registry& registry, const DeferredRendererConfig& config, const PerlinNoise& perlinNoise, const ABuffer* aBuffer)
{
   
    //RenderSkybox(world, registry);
    RenderSkyDome(world, registry, config, perlinNoise);
    RenderSun(world, registry, aBuffer);
   // RenderLensFlares(world, registry);
}

void SkySystem::RenderSkybox(World* world, entt::registry& registry)
{
    auto* renderEngine = RenderEngine::Get();
    auto* camera = world->GetRenderCamera();
    auto view = camera->GetViewMatrix();
    auto proj = camera->GetProjectionMatrix();

    const glm::mat4 skyboxView = glm::mat3(view);//removing translation vector for skybox
    const glm::mat4 skyBoxVP = proj * skyboxView;

    renderEngine->BindShader(skyboxShader);
    renderEngine->SetDepthMask(FALSE);
    renderEngine->SetMat4(skyboxBindings.MVP, skyBoxVP);
    //renderEngine->SetTexture(skybox->texture, GL_TEXTURE_CUBE_MAP, skyboxBindings.skybox, 0); 
    //renderEngine->DrawArrays(GL_TRIANGLES, skybox->mesh.vao, 0, 36);
    renderEngine->SetDepthMask(TRUE);
}

void SkySystem::RenderSkyDome(World* world, entt::registry& registry, const DeferredRendererConfig& config, const PerlinNoise& perlinNoise)
{
    auto* re = RenderEngine::Get();

    re->BindShader(skyDome.shader);
    re->EnableCulling();
    re->EnableDepthTest();
    re->SetDepthFunc(GL_LEQUAL);
    re->SetCullFace(GL_BACK);

    static glm::mat4 model(1.f);
    //model = glm::rotate(model, glm::radians(deltaTime * 0.5f), { 0.58f, 0.58f, 0.58f });
    re->SetMat4(skyDomeBindings.model, model);
    re->SetVec3(skyDomeBindings.botColor, config.sky.botColor);
    re->SetVec3(skyDomeBindings.topColor, config.sky.topColor);
    re->SetVec3(skyDomeBindings.cloudColor, config.sky.cloudColor);

    re->SetTexture(perlinNoise.GetTexture(), GL_TEXTURE_2D, skyDomeBindings.perlinTex, 0);

    for (auto& mesh : skyDome.mesh.GetMeshes())
    {
        re->DrawElements(mesh, GL_TRIANGLES);
    }
    re->EnableCulling();
    re->SetCullFace(GL_FRONT);
    re->SetDepthFunc(GL_LESS);
}

void SkySystem::UpdateLensFlares(World* world, entt::registry& registry)
{
    auto* camera = world->GetRenderCamera();

    const glm::mat4 camView = glm::mat3(camera->GetViewMatrix()); // remove translation
    const glm::mat4 camProj = camera->GetProjectionMatrix();
    auto sunPos = sun.transform.GetPosition();

    auto coords = glm::vec4(sunPos.x, sunPos.y, sunPos.z, 1.f);

    coords = camView * coords;
    coords = camProj * coords;

    coords.x /= coords.w;
    coords.y /= coords.w;

    constexpr glm::vec2 screenCenter{ 0.0f, 0.0f };
    glm::vec2 sunScreenPos = glm::vec2(coords.x, coords.y);

    glm::vec2 toCenter = screenCenter - sunScreenPos;

    if (coords.z < 0.f)
        lensFlares.bClipPass = false;
    else
        lensFlares.bClipPass = true;

    lensFlares.brightness = 1.0f - (glm::length(toCenter) / 0.6f);

    for (int i = 0; i < lensFlares.flares.size(); i++)
    {
        auto& lensFlare = lensFlares.flares[i];
        glm::vec2 direction = toCenter * (float)(i) * lensFlares.spacing;
        lensFlare.pos = sunScreenPos + direction;
    }  
}

void SkySystem::UpdateSun(World* world, float deltatime)
{
    auto camPos = world->GetRenderCamera()->GetPosition();
    auto worldUp = World::GetWorldUp();

    auto sunDir = glm::normalize(dirlight.lightDir);
    auto sunRight = glm::cross(worldUp, sunDir);
    auto sunUp = glm::cross(sunDir, sunRight);

    glm::mat4 sunRot =
    {
        glm::vec4(sunRight, 0.f),
        glm::vec4(sunUp, 0.f),
        glm::vec4(sunDir, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f),
    };

    sun.transform.SetPosition(-sunDir);
    sun.transform.SetRotation(sunRot);
    sun.transform.SetScale({ 0.2f, 0.2f, 0 });
}

void SkySystem::RenderSun(World* world, entt::registry& registry, const ABuffer* aBuffer)
{
    auto* re = RenderEngine::Get();
    auto* camera = world->GetRenderCamera();

    auto view = camera->GetViewMatrix();
    auto proj = camera->GetProjectionMatrix();

    const glm::mat4 sunView = glm::mat3(view);
    const glm::mat4 sunMVP = proj * sunView * sun.transform.GetTransform();

    re->BindShader(sunShader);
    re->EnableDepthTest();
    re->SetDepthFunc(GL_LEQUAL);

    if (aBuffer)
    {
        re->BindImageTexture(0, aBuffer->indexBuffer, 0, false, 0, GL_READ_WRITE, GL_R32UI);
        re->BindACBO(aBuffer->acbo, 0);
    }

    re->SetMat4(sunBindings.MVP, sunMVP);
    
    re->SetTexture(sun.texture, GL_TEXTURE_2D, sunBindings.texture, 0);
    re->SetTexture(sun.innerTexture, GL_TEXTURE_2D, sunBindings.sunInnerTexture, 1);
    
    re->DrawElements(sun.mesh, GL_TRIANGLES);

    re->EnableDepthTest();
}

void SkySystem::RenderLensFlares(World* world, entt::registry& registry)
{
    if (!lensFlares.bClipPass) return;

    auto* renderEngine = RenderEngine::Get();

    renderEngine->BindShader(lensShader);

    renderEngine->EnableBlending();
    renderEngine->DisableDepthTest();

    renderEngine->SetFloat(lensBindings.brightness, lensFlares.brightness);

    for (size_t i = 0; i < lensFlares.flares.size(); i++)
    {
        auto& lensFlare = lensFlares.flares[i];
        renderEngine->SetVec2(lensBindings.pos2D, lensFlare.pos);
        renderEngine->SetVec2(lensBindings.scale2D, lensFlare.scale);
        renderEngine->SetTexture(lensFlare.lensTexture, GL_TEXTURE_2D, lensBindings.lensTexture, 0);
        renderEngine->DrawElements(quadMesh, GL_TRIANGLES);
    }
    
    renderEngine->DisableBlending();
    renderEngine->EnableDepthTest();
}
