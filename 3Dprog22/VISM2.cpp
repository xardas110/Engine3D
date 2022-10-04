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
    CreateTerrain(world);
    //CreateDebugLineExample();
    CreateTextExample(world);
    CreateParticle(world);
}

void VISM2::CreateTerrain(World* world)
{
    //World classen er også en singleton som kan nåes ved World::Get()

    //world->GetTextureManager() er en klasse som har eierskap av alle teksturer, så man trenger ikke å tenke på deletion eller at man laster inn samme tekstur flere ganger

    //texture classen inneholder minimal informasjon som f.eks. tekstur id gitt fra opengl, width height etc.
    //For effektiv caching

    auto* tm = world->GetTextureManager();

    Texture heightmap;
    if (!tm->CreateLazTexture("path", heightmap))
    {
        std::cout << "Failed to create heightmap" << std::endl;
    }

    //VISM2Terrain.json inneholder terreng settings som tessellation nivå, texture layers etc.
    entt::entity terrainId = world->CreateTerrain("../3Dprog22/VISM2Terrain.json");
    //world->CreateTerrain(heightmap, "../3Dprog22/VISM2Terrain.json");
   //funksjonen over tar inn et nytt parameter istede, en heightmap texture som helst burde være kvadratisk. ie. 1024x1024 eller YxY, hvor Y kan være et stort tall

}

void VISM2::CreateParticle(World* world)
{
    auto* tm = world->GetTextureManager();

    auto particle = world->CreateEntity("Particle Emitter");
    auto& emitter = particle.AddComponent<ParticleEmitterComponent>().emitter;
    
    particle.SetPosition(glm::vec3(0.f, 150.f, 0.f));

    if (!tm->LoadTexture("../3Dprog22/Assets/Textures/particle/black smoke/blackSmoke00.png", emitter.particleTexture))
    {
        std::cout << "Failed to load smoke particle" << std::endl;
    }

    emitter.numParticles = 50;
    emitter.lifeTime = 3.1f;
}

void VISM2::CreateDebugLineExample()
{
    auto* rd = RenderDebugger::Get();
    rd->AddDebugSegment(99999.f, glm::vec3(0.f), glm::vec3(0.f, 300.f, 0.f));
    //Støtte for point, box og sphere
    //God performance med instancing
}

void VISM2::CreateTextExample(World* world)
{
    Entity ese = world->CreateEntity("Prosjekt tekst");
    auto& text = ese.AddComponent<TextComponent>().text;
    auto& transform = ese.GetComponent<TransformComponent>();

    transform.SetPosition(glm::vec3(0.f, 100.f, 0.f));
    transform.SetScale(glm::vec3(100.f, 100.f, 100.f));

    text.SetFontType(FontType::Segoe);
    text.SetCoordinateSpace(CoordinateSpace::BillboardSpace);
    text.SetText("VISUM2 PROSJEKT");
    text.SetColor({ 0.f, 0.f, 1.f });
    prosjektTekst = ese.GetEntityId();
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

void VISM2::UpdateEditor(World* world, float deltatime)
{
}

void VISM2::Reset(World* world)
{
}

void VISM2::OnKeyPress(QKeyEvent* event)
{
}


