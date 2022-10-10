#include "PhysicsGameMode.h"
#include "World.h"
#include "Entity.h"
#include "Components.h"
#include "Debug.h"
#include "RenderDebugger.h"
#include "BSpline.h"
#include "water.h"
#include <random>

PhysicsGameMode::~PhysicsGameMode()
{
}

void PhysicsGameMode::Create(World* world, entt::registry& registry)

{
    world->LoadRenderConfig("../3Dprog22/PhysicsWorldConfig.json");
    CreateScene(world);
    CreateBSpline();
   // CreateTree(world);
    //CreateRocks(world);
}

void PhysicsGameMode::BeginPlay(World* world, entt::registry& registry)
{
   
}

void PhysicsGameMode::EndPlay(World* world, entt::registry& registry)
{
}

void PhysicsGameMode::Update(World* world, entt::registry& registry, float deltatime)
{
  
}

void PhysicsGameMode::UpdateEditor(World* world, entt::registry& registry, float deltatime)
{
    auto sm = world->GetStaticMeshManager();

    ImGui::Checkbox("Draw Octree", &bDrawOctreeLeafs);
    ImGui::Checkbox("Draw bounding boxes", &bDrawBoundingBoxes);
    ImGui::Checkbox("Draw Convex Hulls", &bDrawConvexHulls);

    if (bDrawOctreeLeafs) world->physicsSystem->GetOctree()->DrawLeafBounds();
    if (bDrawBoundingBoxes) world->physicsSystem->DrawBoundingBoxes();
    if (bDrawConvexHulls) world->physicsSystem->DrawConvexHulls();
        
    if (ImGui::Button("Spawn boxes 1000"))
    {
        {
            constexpr int iterations = 10;
            constexpr int nrRows = 10;
            constexpr int nrColumns = 10;

            float spacing = 10.f;

            for (auto f = 0; f < iterations; f++)
                for (auto i = 0; i < nrRows; i++)
                {
                    for (auto j = 0; j < nrColumns; j++)
                    {
                        SpawnBox({ i * spacing - (nrRows * spacing * 0.5f), 500.f * (f + 1), j * spacing - (nrColumns * spacing * 0.5f) });
                    }
                };
        }
    }

    if (ImGui::Button("Lag HøyOppløsning terreng"))
    {
        CreateTerrain(world);
    }

    if (ImGui::Button("Spawn Box ground(BIG)"))
    {
        {           
            Entity e = world->CreateEntity("BoxGround");
            auto& collider = e.AddComponent<CollisionComponent>(CollideableType::ConvexHull).col;
            auto& sphereBody = e.AddComponent<PhysicsComponent>();
            auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;

            collider.SetExtents(glm::vec3(1000.f, 1000.f, 1000.f));
            collider.SetPosition(glm::vec3(0.f, 0.f, 0.f));

            sm->LoadStaticMesh("../3Dprog22/Assets/Models/Box/Box.obj", boxMesh, true, true);
            const MeshData* temp = sm->GetMeshData("../3Dprog22/Assets/Models/Box/Box.obj");

            std::cout << "meshdata verts " << temp->verts.size() << std::endl;

            collider.BuildConvex(temp->verts.data(), temp->verts.size());

            boxMesh.bCastShadow = false;
            boxMesh.SetColor(glm::vec3(1.0f, 0.0f, 0.0f));

            e.SetScale({ 1000.0f, 1000.0f, 1000.0f });
            e.SetPosition({ 0.f, -350.f, 0.f });

            sphereBody.body.SetMass(0.f);
        }
    }

    if (ImGui::Button("Spawn balls 2500"))
    {
        int iterations = 1;
        int nrRows = 50;
        int nrColumns = 50;
        constexpr float spacing = 10.f;
        for (auto f = 0; f < iterations; f++)
            for (auto i = 0; i < nrRows; i++)
            {
                for (auto j = 0; j < nrColumns; j++)
                {
                    Entity e = world->CreateEntity("Sphere" + std::to_string(i * nrColumns + j));
                    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::Sphere).col;
                    auto& sphereBody = e.AddComponent<PhysicsComponent>();
                    auto& bSpline = e.AddComponent<BSplines>();
                    auto& sphereMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
                    collider.SetExtents(glm::vec3(4.8f, 4.8f, 4.8f));
                    collider.SetPosition(glm::vec3(0.f, 0.f, 0.f));
                    sm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere_LOD4.obj", sphereMesh);
                    sphereBody.body.SetMass(1000.f);
                    e.SetPosition({ i * spacing - (nrRows * spacing * 0.5f), 500.f * (f + 1), j * spacing - (nrColumns * spacing * 0.5f) });
                    e.SetScale({ 5.f, 5.f, 5.f });
                }
            };
    }
    if (ImGui::Button("Spawn capsules 1000"))
    {
        int iterations = 10;
        int nrRows = 10;
        int nrColumns = 10;
        constexpr float spacing = 50.f;
        for (auto f = 0; f < iterations; f++)
            for (auto i = 0; i < nrRows; i++)
            {
                for (auto j = 0; j < nrColumns; j++)
                {
                    SpawnCapsule({ i * spacing - (nrRows * spacing * 0.5f), 500.f * (f + 1), j * spacing - (nrColumns * spacing * 0.5f) });
                }
            };
    }

    if (ImGui::Button("Spawn Statues 100"))
    {
        int iterations = 1;
        int nrRows = 10;
        int nrColumns = 10;
        constexpr float spacing = 50.f;
        for (auto f = 0; f < iterations; f++)
            for (auto i = 0; i < nrRows; i++)
            {
                for (auto j = 0; j < nrColumns; j++)
                {
                    SpawnStatue({ i * spacing - (nrRows * spacing * 0.5f), 500.f * (f + 1), j * spacing - (nrColumns * spacing * 0.5f) });
                }
            };
    }

    /*
    int chsSize = (int)chs.size();

    for (int i = 0; i < chsSize - 1; i++)
    {
        for (int j = i + 1; j < chsSize; j++)
        {
            auto& c1 = chs[i];
            auto& c2 = chs[j];

            ContactManifold contact;
            if (GJK::IntersectEPA(&c1, &c2, contact))
            { 
                std::cout << "SEP DIST: " << contact.seperationDistance << std::endl;
                VEC3(contact.normal);
            }
        }
    }
    */
}

void PhysicsGameMode::Reset(World* world, entt::registry& registry)
{
}

void PhysicsGameMode::OnKeyPress(QKeyEvent* event)
{
    
    if (Qt::Key_F == event->key())
    {
        SpawnBox(World::Get()->GetRenderCamera()->GetPosition());
    }

    if (Qt::Key_E == event->key())
    {    
        SpawnCapsule(World::Get()->GetRenderCamera()->GetPosition());
    }

    if (Qt::Key_T == event->key())
    {
        SpawnStatue(World::Get()->GetRenderCamera()->GetPosition());
        // SpawnConvexHull();
    }

    if (Qt::Key_R == event->key())
    {
        SpawnActualSphere(World::Get()->GetRenderCamera()->GetPosition());
        // SpawnConvexHull();
    }
    
}

void PhysicsGameMode::SpawnBox(const glm::vec3& pos)
{
    auto* world = World::Get();
    auto* sm = world->GetStaticMeshManager();

    Entity e = world->CreateEntity("Box");
    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::ConvexHull).col;
    auto& body = e.AddComponent<PhysicsComponent>().body;
    auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
    sm->LoadStaticMesh("../3Dprog22/Assets/Models/Box/Box.obj", boxMesh, true, false);

    collider.SetConvexHull(sm->GetConvexHull("../3Dprog22/Assets/Models/Box/Box.obj"));

    collider.SetExtents(glm::vec3(5.f));
    boxMesh.SetColor(glm::vec3(float(rand() % 10) / 10.f, float(rand() % 10) / 10.f, float(rand() % 10) / 10.f));

    body.SetMass(1000.f);
    e.SetPosition(pos);
    e.SetScale({ 5.f, 5.f, 5.f });
}

void PhysicsGameMode::SpawnTriangle(const glm::vec3& pos)
{
    auto* world = World::Get();
    auto* sm = world->GetStaticMeshManager();

    Entity e = world->CreateEntity("Box");
    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::ConvexHull).col;
    auto& body = e.AddComponent<PhysicsComponent>().body;
    auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
    sm->LoadStaticMesh("../3Dprog22/Assets/Models/Rock1/Rock1.obj", boxMesh);
    const MeshData* temp = sm->GetMeshData("../3Dprog22/Assets/Models/Rock1/Rock1.obj");

    collider.BuildKDOP(temp->verts.data(), temp->verts.size(), KDOP::DOP26, KDOP_AXIS::Y);
    collider.SetExtents(glm::vec3(5.f));

    body.SetMass(50.f);
    e.SetPosition(pos);
    e.SetScale({ 5.f, 5.f, 5.f });
}

void PhysicsGameMode::SpawnStatue(const glm::vec3& pos)
{
    auto* world = World::Get();
    auto* sm = world->GetStaticMeshManager();

    Entity e = world->CreateEntity("Box");
    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::ConvexHull).col;
    auto& body = e.AddComponent<PhysicsComponent>().body;
    auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
    sm->LoadStaticMesh("../3Dprog22/Assets/Models/Statue/statue.obj", boxMesh, true, false);
    collider.SetConvexHull(sm->GetConvexHull("../3Dprog22/Assets/Models/Statue/statue.obj"));
    collider.SetExtents(glm::vec3(1.f));
    
    body.SetMass(1000.f);

    e.SetPosition(pos);
    e.SetScale({ 1.f, 1.f, 1.f });
}

void PhysicsGameMode::SpawnCapsule(const glm::vec3& pos)
{
    auto* world = World::Get();
    auto* sm = world->GetStaticMeshManager();
    static int numCapsules{ 0 };
    Entity e = world->CreateEntity("Capsule" + std::to_string(numCapsules++));
    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::Capsule).col;
    auto& body = e.AddComponent<PhysicsComponent>().body;
    auto& boxMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
    sm->LoadStaticMesh("../3Dprog22/Assets/Models/Capsule/Capsule.obj", boxMesh);
    boxMesh.SetColor({ 0.7f, 0.5f, 0.f });
    body.SetMass(50.f);
    body.SetElasticity(0.5f);
    collider.SetExtents(glm::vec3(5.0f));
    e.SetPosition(pos);
    e.SetScale({ 5.f, 5.f, 5.f });
}

void PhysicsGameMode::SpawnActualSphere(const glm::vec3& pos)
{
    auto world = World::Get();
    Entity e = world->CreateEntity("test");
    auto sm = world->GetStaticMeshManager();
    auto& collider = e.AddComponent<CollisionComponent>(CollideableType::Sphere).col;
    auto& sphereBody = e.AddComponent<PhysicsComponent>();
    auto& bSpline = e.AddComponent<BSplines>();
    auto& sphereMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
    collider.SetExtents(glm::vec3(4.8f, 4.8f, 4.8f));
    collider.SetPosition(glm::vec3(0.f, 0.f, 0.f));
    sm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere.obj", sphereMesh);
    sphereBody.body.SetMass(1000.f);
    e.SetPosition(pos);
    e.SetScale({ 5.f, 5.f, 5.f });
}

void PhysicsGameMode::CreateTerrain(World* world)
{
    entt::entity terrainId = world->CreateTerrain("../3Dprog22/VISM2TerrainHighRes.json");

    Entity ent(terrainId, world);
    auto& terrain = ent.GetComponent<TerrainComponent>().terrain;
    auto& config = terrain.config;
    CollisionHeightmap heightmap;

    glm::vec3 scale(config.scaleXZ, config.scaleY, config.scaleXZ);

    heightmap.SetHeightmap(config.N, config.N, scale, &terrain.heightmapBuffer);

    world->physicsSystem->RegisterHeightmap(heightmap);
}

void PhysicsGameMode::CreateWater(World* world)
{
   auto tm = world->GetTextureManager();
   Entity waterEntity = world->CreateEntity("Water");
   auto& water = waterEntity.AddComponent<Water>();
   auto& trans = waterEntity.GetComponent<TransformComponent>();
   trans.SetScale(glm::vec3(10.f));
   trans.SetPosition(glm::vec3(0.f, 0.f, 0.f));
   tm->LoadTexture("../3Dprog22/Assets/Textures/Water/normalmap.png", water.normalmap);
   tm->LoadTexture("../3Dprog22/Assets/Textures/Water/waterDUDV.png", water.waterDUDV);
}

void PhysicsGameMode::CreateTree(World* world)
{
    auto* sm = world->GetStaticMeshManager();
    std::uniform_real_distribution<float> rf(-800.f, 800.f);
    std::default_random_engine engine;
    {
        Entity tree = world->CreateEntity("TreeInstanced");
        auto& instance = tree.AddComponent<StaticMeshInstancedComponent>().staticMeshInstanced;

        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod0.obj", instance.LOD[0]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod1.obj", instance.LOD[1]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod2.obj", instance.LOD[2]);
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod3.obj", instance.LOD[3]);

        ConvexHull hull = sm->GetConvexHull("../3Dprog22/Assets/Models/Tree2/Lod0.obj");

        for (size_t i = 0; i < 1000; i++)
        {
            Entity colEntity = world->CreateEntity("TreeCol" + std::to_string(i));
            auto& transform = colEntity.GetComponent<TransformComponent>();
            auto& body = colEntity.AddComponent<PhysicsComponent>().body;
            body.SetMass(0.f);
            auto& col = colEntity.AddComponent<CollisionComponent>(CollideableType::ConvexHull).col;
            col.SetConvexHull(hull);

            Transform trans;
            float x = rf(engine);
            float z = rf(engine);
            float y = world->GetTerrainHeightAt(x, z);
            float ns = world->GetTerrainSlopeAt(x, z);
            if (ns < 0.6f) continue;

            trans.SetPosition({ x, y, z });
            int randScale = rand() % 2 + 2;
            trans.SetScale({ randScale, randScale, randScale });
            glm::mat4 rot(1.f);
            rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));
            trans.SetRotation(glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f)));

            transform.SetTransform(trans.GetTransform());
            col.SetPosition(trans.GetPosition());
            col.SetExtents(trans.GetScale());
            col.SetRotation(trans.GetRotation());

            instance.transforms.emplace_back(trans.GetTransform());
        }
    }
}

void PhysicsGameMode::CreateRocks(World* world)
{
    auto* sm = world->GetStaticMeshManager();
    std::uniform_real_distribution<float> rf(-800.f, 800.f);
    std::default_random_engine engine;
    {
        StaticMesh staticMesh;
        sm->LoadStaticMesh("../3Dprog22/Assets/Models/Rock1/Rock1.obj", staticMesh);
        ConvexHull hull = sm->GetConvexHull("../3Dprog22/Assets/Models/Rock1/Rock1.obj");

        for (size_t i = 0; i < 1000; i++)
        {
            Entity rock = world->CreateEntity("Rock");
            auto& mesh = rock.AddComponent<StaticMeshComponent>().staticMeshInstanced;
            auto& transform = rock.GetComponent<TransformComponent>();
            auto& body = rock.AddComponent<PhysicsComponent>().body;
            auto& col = rock.AddComponent<CollisionComponent>(CollideableType::ConvexHull).col;
            body.SetMass(0.f);
            col.SetConvexHull(hull);
            mesh = staticMesh;

            Transform trans;
            float x = rf(engine);
            float z = rf(engine);
            float y = world->GetTerrainHeightAt(x, z);
            float ns = world->GetTerrainSlopeAt(x, z);
            if (ns < 0.8f) continue;

            trans.SetPosition({ x, y, z });
            int randScale = rand() % 2 + 2;
            trans.SetScale({ randScale, randScale, randScale });
            glm::mat4 rot(1.f);
            rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));
            trans.SetRotation(glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f)));

            transform.SetTransform(trans.GetTransform());
            col.SetPosition(trans.GetPosition());
            col.SetExtents(trans.GetScale());
            col.SetRotation(trans.GetRotation());
        }
    }
}

void PhysicsGameMode::CreateBSpline()
{
    BSpline bSpline;
}

void PhysicsGameMode::CreateScene(World* world)
{
    CreateWater(world);
    auto sm = world->GetStaticMeshManager();
    auto tm = world->GetTextureManager();
    float elasity = 0.2f;

    /*
    {
        Entity e = world->CreateEntity("SphereGround");
        auto& collider = e.AddComponent<CollisionComponent>(CollideableType::Sphere).col;
        auto& sphereBody = e.AddComponent<PhysicsComponent>();
        auto& sphereMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;

        collider.SetExtents(glm::vec3(1000.f, 1000.f, 1000.f));
        collider.SetPosition(glm::vec3(0.f, 0.f, 0.f));

        sphereMesh.AddMesh(world->GetMeshManager()->GetOctahedronBallMesh());
        sphereMesh.bCastShadow = false;
        sphereMesh.SetColor(glm::vec3(0.5f, 0.5f, 0.2f));

        e.SetScale({ 1000.0f, 1000.0f, 1000.0f });
        e.SetPosition({ 0.f, -1000.f, 0.f });

        sphereBody.body.SetMass(0.f);
    }

   
    constexpr int iterations = 1;
    constexpr int nrRows = 5;
    constexpr int nrColumns = 5;

    for (auto f = 0; f < iterations; f++)
        for (auto i = 0; i < nrRows; i++)
        {
            for (auto j = 0; j < nrColumns; j++)
            {
                Entity e = world->CreateEntity("test");
                auto& collider = e.AddComponent<CollisionComponent>(CollideableType::Sphere).col;
                auto& sphereBody = e.AddComponent<PhysicsComponent>();
                auto& sphereMesh = e.AddComponent<StaticMeshComponent>().staticMeshInstanced;
                collider.SetExtents(glm::vec3(4.8f, 4.8f, 4.8f));
                collider.SetPosition(glm::vec3(0.f, 0.f, 0.f));
                sm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere.obj", sphereMesh);
                sphereBody.body.SetMass(1000.f);
                e.SetPosition({ i * 2.f - nrRows, 500.f * (f + 1), j * 2.f - nrColumns });
                e.SetScale({ 5.f, 5.f, 5.f });
            }
        };
        */
    /*
    for (auto f = 0; f < 1; f++)
        for (auto i = 0; i < nrRows; i++)    
            for (auto j = 0; j < nrColumns; j++)
            {
                SpawnBox({ i * 2.f - nrRows, 500.f * (f + 1), j * 2.f - nrColumns });
            }
        
    for (auto f = 0; f < 1; f++)
        for (auto i = 0; i < 5; i++)
            for (auto j = 0; j < 5; j++)
            {
                SpawnTriangle({ i * 2.f - 5, 2000.f * (f + 1), j * 2.f - 5 });
            }

    for (auto f = 0; f < 1; f++)
        for (auto i = 0; i < 5; i++)
            for (auto j = 0; j < 5; j++)
            {
                SpawnSphere({ i * 2.f - 5, 1250.f * (f + 1), j * 2.f - 5 });
            }
    */
}
