#include "EksamenGameMode.h"
#include "World.h"
#include "Entity.h"
#include "RenderDebugger.h"
#include "Character.h"
#include "AiCharacter.h"
#include "PointGenerator.h"
#include "Debug.h"
#include "Include/glm/glm.hpp"
#include "Trophy.h"
#include "AudioManager.h"
#include "MaterialManager.h"
#include "AudioManager.h"
#include "HelperMath.h"
#include "RenderEngine.h"
#include <iostream>
#include <random>

static float timeSinceLastExp{ 0.f };

struct Entity;

EksamenGameMode::~EksamenGameMode()
{
	DeleteSoundFiles();
}

void EksamenGameMode::Create(World* world, entt::registry& registry)
{
	world->PreLoadAssets("./Assets/Paths.json");

	CreateTerrain(world, registry);
	CreateHouse(world);
	CreatePlants(world);
	
	LoadAmbientSound(world);
	LoadSoundFiles();

	InitBezier();
	CreateSun(world);	
	CreateBomber(world, registry);
	CreateBomber2(world, registry);
	CreateTophies(world);
	CreateStatusBillboard(world);
	CreateBaricades(world);
	CreateTrees(world);
		
	//CreateParticles(world);

	CreatePlayer(world, registry);
	CreateAiCharacter(world, registry);
	
}

void EksamenGameMode::Update(World* world, entt::registry& registry, float deltatime)
{	
	//Oppgave 13 -> alt annet kjørte ikke i editor modus fra før
	UpdateSun(world, deltatime);
	UpdateBomber(world, registry, deltatime);
	UpdateBomber2(world, registry, deltatime);
	UpdateDroppedBombs(world, deltatime);
	UpdateTrophies(world);
	UppdateGameStatus();
	UpdateStatusBillboard(world, deltatime);	

	timeSinceLastExp += deltatime;	
}

void EksamenGameMode::UpdateEditor(World* world, entt::registry& registry, float deltatime)
{
	UpdatePlayer(world, deltatime);
	//DrawBezier();
	UpdateAiCharacter(world, deltatime);
}

void EksamenGameMode::Reset(World* world, entt::registry& registry)
{
	ColorTrophies(world);
}

void EksamenGameMode::CreatePlayer(World* world, entt::registry& registry)
{
	//Oppgave 4 lager en spiller ved bruk at scripting
	//Se i klassen Character for alle endringene
	{
		Entity player = world->CreateEntity("Player");
		player.AddComponent<ScriptComponent>().Bind<Character>();
		playerEnt = player.GetEntityId();

		glm::vec3 pPos = {700.f, 0.f, 700.f};

		float y = world->GetTerrainHeightAt(pPos.x, pPos.z);
		pPos.y = y + 10.f;

		player.SetPosition(pPos);
	}
}

void EksamenGameMode::UpdatePlayer(World* world, float deltatime)
{
	//Oppgave 4 oppdaterer spiller over heightmappet
	Entity playerEntity(playerEnt, world);
	auto pos = playerEntity.GetPosition();
	pos.y = world->GetTerrainHeightAt(pos.x, pos.z) + 11.f;
	playerEntity.SetPosition(pos);
}

void EksamenGameMode::CreateBomber(World* world, entt::registry& registry)
{
	//Oppgave 3 
	auto* smm = world->GetStaticMeshManager();
	Entity bomber = world->CreateEntity("Bomber");
	bomberEnt = bomber.GetEntityId();
	auto& bomberSM = bomber.AddComponent<StaticMeshComponent>().staticMeshInstanced;
	smm->LoadStaticMesh("../3Dprog22/Assets/Models/SpaceCraft/E 45 Aircraft_obj.obj", bomberSM);
	bomber.SetScale({10.f, 10.f, 10.f});

	//cache bombs
	StaticMesh sm;
	smm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere.obj", sm);
}

void EksamenGameMode::CreateBomber2(World* world, entt::registry& registry)
{
	auto* smm = world->GetStaticMeshManager();

	Entity bomber = world->CreateEntity("Bomber2");
	bomberEnt2 = bomber.GetEntityId();
	auto& bomberSM = bomber.AddComponent<StaticMeshComponent>().staticMeshInstanced;
	smm->LoadStaticMesh("../3Dprog22/Assets/Models/SpaceCraft/E 45 Aircraft_obj.obj", bomberSM);
	bomber.SetScale({ 25.f, 25.f, 25.f });
}

void EksamenGameMode::UpdateBomber(World* world, entt::registry& registry, float deltatime)
{
	Entity bomber(bomberEnt, world);

	static float currentX;
	static float startX = 0.f;
	static float endX = 1.f;
	static int sign{1};
	static float step = 0.05f;
	static float bombTimer = 2.f;
	static float currentBombTimer{ 0.f };

	if (currentX >= endX - step && sign == 1) { sign = -1; } // den skal slutte en del før
	if (currentX <= startX + step && sign == -1) { sign = 1; }

	{
		currentX += step * (float)sign * deltatime;
		glm::vec2 bezierPos = bezier.Func(currentX);
		glm::vec4 pos4D = glm::vec4(bezierPos.x, 0.f, bezierPos.y, 1.f);
		glm::vec3 finalPos = bezier.GetTransform() * pos4D;
		bomber.SetPosition(finalPos);
	}
	{
		auto nextX = currentX += step * (float)sign * deltatime;
		glm::vec2 bezierPos = bezier.Func(nextX);
		glm::vec4 pos4D = glm::vec4(bezierPos.x, 0.f, bezierPos.y, 1.f);
		glm::vec3 finalPos = bezier.GetTransform() * pos4D;

		glm::vec3 dir = finalPos - bomber.GetPosition();
		dir.y = 0.f;
		dir = glm::normalize(-dir);

		glm::vec3 right = glm::cross(World::GetWorldUp(), dir);

		glm::mat4 view =
		{
			{right, 0.f},
			{World::GetWorldUp(), 0.f},
			{dir, 0.f},
			{0.f, 0.f, 0.f, 1.f},
		};

		bomber.SetRotation(view);

	}
	if (currentBombTimer >= 2.f)
	{
		DropBomb(world, bomber.GetPosition());
		currentBombTimer = 0.f;
	}
	currentBombTimer += deltatime;
}

void EksamenGameMode::UpdateBomber2(World* world, entt::registry& registry, float deltatime)
{
	Entity bomber(bomberEnt2, world);

	static float currentX{0.f};
	static float startX = 0.f;
	static float endX = 1.f;
	static int sign{ 1 };
	static float step = 0.05f;
	static float bombTimer = 2.f;
	static float currentBombTimer{ 0.f };

	if (currentX >= endX - step && sign == 1) { sign = -1; } // den skal slutte en del før
	if (currentX <= startX + step && sign == -1) { sign = 1; }

	{
		currentX += step * (float)sign * deltatime;
		glm::vec2 bezierPos = bezier2.Func(currentX);
		glm::vec4 pos4D = glm::vec4(bezierPos.x, 0.f, bezierPos.y, 1.f);
		glm::vec3 finalPos = bezier2.GetTransform() * pos4D;
		bomber.SetPosition(finalPos);
	}
	{
		auto nextX = currentX += step * (float)sign * deltatime;
		glm::vec2 bezierPos = bezier2.Func(nextX);
		glm::vec4 pos4D = glm::vec4(bezierPos.x, 0.f, bezierPos.y, 1.f);
		glm::vec3 finalPos = bezier2.GetTransform() * pos4D;

		glm::vec3 dir = finalPos - bomber.GetPosition();
		dir.y = 0.f;
		dir = glm::normalize(-dir);

		glm::vec3 right = glm::cross(World::GetWorldUp(), dir);

		glm::mat4 view =
		{
			{right, 0.f},
			{World::GetWorldUp(), 0.f},
			{dir, 0.f},
			{0.f, 0.f, 0.f, 1.f},
		};

		bomber.SetRotation(view);

	}
	if (currentBombTimer >= 10.f)
	{
		DropBomb(world, bomber.GetPosition());
		currentBombTimer = 0.f;
	}
	currentBombTimer += deltatime;
}

void EksamenGameMode::DropBomb(World* world, const glm::vec3& bomberPos)
{
	/*
	auto* smm = world->GetStaticMeshManager();

	static size_t nextNameVal{ 0 };
	Entity bomb = world->CreateEntity("Bomb" + std::to_string(nextNameVal));
	nextNameVal++;

	auto& sm = bomb.AddComponent<StaticMeshComponent>().staticMeshInstanced;
	auto& collision = bomb.AddComponent<OBBCollisionComponent>((std::uint32_t)bomb.GetEntityId()).collisionVolume;
	collision.SetExtents({ 10.f, 10.f, 10.f });
	auto& rigidBody = bomb.AddComponent<PhysicsComponent>().rigidBody;

	rigidBody.SetMass(100.f);
	rigidBody.friction = 0.f;
	rigidBody.bFixedRotation = true;
	//Will refactor this later
	rigidBody.inverseInertiaTensor = PhysicsHelper::GetInertiaTensor(&collision, &rigidBody);

	int x = rand() % 2;
	int z = rand() % 2;

	glm::vec3 dir = { x, 1.f, z };
	dir = glm::normalize(dir);
	float force = rand() % 2000 + 100;

	rigidBody.velocity += dir * force;

	smm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/Sphere.obj", sm);
	bomb.SetPosition(bomberPos);
	bomb.SetScale({ 5.f, 5.f, 5.f });
	bombs.push_back({0.f,  bomb });
	*/
}

void EksamenGameMode::UpdateDroppedBombs(World* world, float deltatime)
{
	for (size_t i = 0; i < bombs.size(); i++)
	{	
		auto& bomb = bombs[i];
		
		if (!bomb.second.IsValid()) continue;
	
		auto& t = bomb.first;
		auto& b = bomb.second;
		
		if (t > 10.f)
		{
			ExplodeBomb(world, b, deltatime);
		}
		else
		{
			BombStage1(b, deltatime);
		}
		t += deltatime;
		
		glm::vec3 newPos = b.GetPosition();
		float y = world->GetTerrainHeightAt(newPos.x, newPos.z) + b.GetScale().y;

		if (newPos.y < y)
			newPos.y = y;

		b.SetPosition(newPos);
	}
}

void EksamenGameMode::BombStage1(Entity& bomb, float deltatime)
{	
	auto scale = bomb.GetScale();
	float factor = deltatime * 0.5f;
	bomb.SetScale(scale + factor);
}

void EksamenGameMode::BombStage2(Entity& bomb, float deltatime)
{
	/*
	auto scale = bomb.GetScale();
	float factor = deltatime * 0.8f;
	bomb.SetScale(scale + factor);
	
	auto& sm = bomb.GetComponent<StaticMeshComponent>().staticMeshInstanced;
	auto& col = bomb.GetComponent<OBBCollisionComponent>().collisionVolume;
	col.SetExtents(bomb.GetScale());

	int sign = rand() % 4;

	if (sign)
		sm.SetColor({ 1.f, 0.f, 0.f });
	else
		sm.SetColor({ 0.5f, 0.5f, 0.5f });
		*/
}

void EksamenGameMode::BombStage3(Entity& bomb, float deltatime)
{
	/*
	auto scale = bomb.GetScale();
	float factor = deltatime * 1.8f;
	bomb.SetScale(scale + factor);

	auto& sm = bomb.GetComponent<StaticMeshComponent>().staticMeshInstanced;
	auto& col = bomb.GetComponent<OBBCollisionComponent>().collisionVolume;

	col.SetExtents(bomb.GetScale());

	int sign = rand() % 2;

	if (sign)
		sm.SetColor({ 1.f, 0.f, 0.f });
	else
		sm.SetColor({ 1.f, 1.f, 1.f });
		*/
}

void EksamenGameMode::ExplodeBomb(World* world, Entity& bomb, float deltatime)
{
	static float waitTime{ 2.f };

	if (timeSinceLastExp < waitTime)
	{ 
		world->DeleteEntity(bomb.GetEntityId());
		return;
	}

	waitTime = waitTime + (rand() % 3);
	timeSinceLastExp = 0.f;

	explosion.SetPosition(bomb.GetPosition());
	explosion.SetPitch(1.f);
	explosion.SetGain(1.f);
	explosion.Play();

	world->DeleteEntity(bomb.GetEntityId());
}

void EksamenGameMode::CreateAiCharacter(World* world, entt::registry& registry)
{
	// oppgave 9
	{
		Entity aiCharacter = world->CreateEntity("AiCharacter");
		aiCharacter.AddComponent<ScriptComponent>().Bind<AiCharacter>();
		aiEnt = aiCharacter.GetEntityId();

		glm::vec3 pPos = { 700.f, 0.f, 700.f };

		float y = world->GetTerrainHeightAt(pPos.x, pPos.z);
		pPos.y = y + 10.f;
		aiCharacter.SetPosition(pPos);
	}
}

void EksamenGameMode::UpdateAiCharacter(World* world, float deltatime)
{
	// oppgave 9
	Entity ai(aiEnt, world);
	auto pos = ai.GetPosition();
	pos.y = world->GetTerrainHeightAt(pos.x, pos.z) + 11.f;
	ai.SetPosition(pos);

	float closestTrophy{ FLT_MAX };
	entt::entity trophyId{entt::null};

	for (size_t i = 0; i < enemyTrophies.size(); i++)
	{
		auto& trophy = enemyTrophies[i];

		if (!trophy.IsValid()) continue;

		auto dist = glm::min(closestTrophy, glm::length(ai.GetPosition() - trophy.GetPosition()));

		if (dist < closestTrophy)
		{ 
			closestTrophy = dist;
			trophyId = trophy.GetEntityId();
		}
	}

	auto* instance = ai.GetComponent<ScriptComponent>().instance;
	auto* aiCharacter = reinterpret_cast<AiCharacter*>(instance);

	if (instance)
		if (aiCharacter)
		{
			Entity selectedTrophy(trophyId, world);
			if (!selectedTrophy.IsValid()) return;

			//Oppgave 15
			aiCharacter->SetMovementSpeed(70.f);
			aiCharacter->SetStopFollowDist2(100.f);
			aiCharacter->MoveTo(selectedTrophy.GetPosition());
		};
}

void EksamenGameMode::CreateTophies(World* world)
{
	{//trophies
		for (auto i = 0; i < 10; i++)
		{
			Entity trophyEnt = world->CreateEntity("PlayerTrophy" + std::to_string(i));
			trophyEnt.AddComponent<ScriptComponent>().Bind<Trophy>();

			float x = rand() % 1000 - 500;
			float z = rand() % 1000 - 500;
			float y = world->GetTerrainHeightAt(x, z) + 10.f;

			trophyEnt.SetPosition({x,y,z});	

			playerTrophies.emplace_back(trophyEnt);
		}
		
		for (auto i = 0; i < 10; i++)
		{
			Entity trophyEnt = world->CreateEntity("EnemyTrophy" + std::to_string(i));
			trophyEnt.AddComponent<ScriptComponent>().Bind<Trophy>();

			float x = rand() % 1000 - 500;
			float z = rand() % 1000 - 500;
			float y = world->GetTerrainHeightAt(x, z) + 10.f;

			trophyEnt.SetPosition({ x,y,z });

			enemyTrophies.emplace_back(trophyEnt);
		}	
	}
}

void EksamenGameMode::ColorTrophies(World* world)
{
	for (size_t i = 0; i < playerTrophies.size(); i++)
	{
		auto& trophyEnt = playerTrophies[i];

		if (!trophyEnt.IsValid()) continue;

		auto* instance = trophyEnt.GetComponent<ScriptComponent>().instance;
		auto* trophy = reinterpret_cast<Trophy*>(instance);

		if (trophy)
		{
			trophy->SetTrophyColor({ 1.f, 0.f, 0.f });
		}
	}

	for (size_t i = 0; i < enemyTrophies.size(); i++)
	{
		auto& trophyEnt = enemyTrophies[i];

		if (!trophyEnt.IsValid()) continue;

		auto* instance = trophyEnt.GetComponent<ScriptComponent>().instance;
		auto* trophy = reinterpret_cast<Trophy*>(instance);

		if (trophy)
		{
			trophy->SetTrophyColor({ 0.f, 0.f, 1.f });
		}
	}
}

void EksamenGameMode::UpdateTrophies(World* world)
{
	for (auto it = playerTrophies.begin(); it != playerTrophies.end();)
	{
		auto& trophyEnt = *it;

		if (!trophyEnt.IsValid())
		{
			it = playerTrophies.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto it = enemyTrophies.begin(); it != enemyTrophies.end();)
	{
		auto& trophyEnt = *it;

		if (!trophyEnt.IsValid())
		{
			it = enemyTrophies.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void EksamenGameMode::FreeTrophy(World* world)
{
	/*
	for (size_t i = 0; i < enemyTrophies.size(); i++)
	{
		auto& trophy = enemyTrophies[i];
		if (!trophy.IsValid()) continue;

		auto& trophyCol = trophy.GetComponent<OBBCollisionComponent>();
		while (world->pathFindingSystem.IsOccupied(&trophyCol.collisionVolume))
		{
			float x = rand() % 1000 - 500;
			float z = rand() %	1000 - 500;
			float y = world->GetTerrainHeightAt(x, z) + 10.f;
			trophy.SetPosition({ x, y, z });
		}
	}
	*/
}

void EksamenGameMode::CreateBaricades(World* world)
{
	/*
	//Oppgave 10
	//vfvndg3cb_LOD0
	auto* sm = world->GetStaticMeshManager();
	auto* rd = RenderDebugger::Get();

	int mapArea = 1000;

	for (size_t i = 0; i < 10; i++)
	{
		Entity wall = world->CreateEntity("Wall"+std::to_string(i));
		auto& smm = wall.AddComponent<StaticMeshComponent>();
		auto& col = wall.AddComponent<OBBCollisionComponent>((std::uint32_t)wall.GetEntityId());
		// will be untextured
		sm->LoadStaticMesh("../3Dprog22/Assets/Models/StoneWall/StoneWall.obj", smm);

		wall.SetScale({ 1.0f, 1.f, 1.f });
		col.collisionVolume.SetExtents({ 75.f, 35.f, 15.f });
		col.collisionVolume.SetLocalPos({ 0.f, 30.f, 0.f });
		

		while (world->collisionSystem.Intersect(&col.collisionVolume))
		{
			float x = rand() % mapArea - 500;
			float z = rand() % mapArea - 500;
			float y = world->GetTerrainHeightAt(x, z);
			float slope = world->GetTerrainSlopeAt(x, z);

			if (slope < 0.6f) continue;

			glm::quat rot = glm::identity<glm::quat>();
			rot = glm::rotate(rot, glm::radians(float(rand() % 90)), { 0.f, 1.f, 0.f });

			wall.SetRotation(rot);
			wall.SetPosition({ x, y - 20.f, z });
		}

		barricades.emplace_back(wall);
	}
	*/
}

void EksamenGameMode::CreateStatusBillboard(World* world)
{
	{
		Entity billEnt = world->CreateEntity("Billboard Text");
		billEnt.SetScale({ 50.f, 50.f, 50.f });
		statusBillboardEnt = billEnt.GetEntityId();
		auto& text = billEnt.AddComponent<TextComponent>().text;
		text.SetFontType(FontType::Segoe);
		text.SetCoordinateSpace(CoordinateSpace::BillboardSpace);
		text.SetText("");
		text.SetHidden(true);
		text.SetTextOffset({ 0.45f, 0.5f });
	}
	{
		Entity pse = world->CreateEntity("Player Score");
		playerScoreEnt = pse.GetEntityId();
		auto& text = pse.AddComponent<TextComponent>().text;
		text.SetFontType(FontType::Segoe);
		text.SetCoordinateSpace(CoordinateSpace::LocalSpace);
		text.SetText("PScore: 0");
		text.SetColor({ 1.f, 0.f, 0.f });
		text.SetTextOffset({ 0.05f, 0.9f });
	}
	{
		Entity ese = world->CreateEntity("Enemy Score");
		enemyScoreEnt = ese.GetEntityId();
		auto& text = ese.AddComponent<TextComponent>().text;
		text.SetFontType(FontType::Segoe);
		text.SetCoordinateSpace(CoordinateSpace::LocalSpace);
		text.SetText("EScore: 0");
		text.SetColor({0.f, 0.f, 1.f});
		text.SetTextOffset({ 0.05f, 0.8f });
	}
}

void EksamenGameMode::UpdateStatusBillboard(World* world, float deltatime)
{
	Entity billboard(statusBillboardEnt, world);

	if (!billboard.IsValid()) return;

	Entity pe(playerEnt, world);

	billboard.SetPosition(pe.GetPosition() + glm::vec3(0.f, 20.f, 0.f));

	auto& text = billboard.GetComponent<TextComponent>().text;

	switch (gameStatus)
	{
	case PlayerWin:
	{
		text.SetColor({ 0.f, 1.f, 0.f });
		text.SetText("PLAYER WINS!");
		text.SetHidden(false);
	}
	break;
	case AiWin:
	{
		text.SetColor({ 1.f, 0.f, 0.f });
		text.SetText("AI WINS!");
		text.SetHidden(false);
	}
	break;
	case Draw:
	{
		text.SetColor({ 1.f, 0.f, 1.f });
		text.SetText("DRAW!");
		text.SetHidden(false);
	}
	break;
	default:
		break;
	}

	{
		Entity ps(playerScoreEnt, world);
		auto& text = ps.GetComponent<TextComponent>().text;

		int pScore = 10 - playerTrophies.size();
		text.SetText("PScore: " + std::to_string(pScore));
	}
	{
		Entity es(enemyScoreEnt, world);
		auto& text = es.GetComponent<TextComponent>().text;

		int eScore = 10 - enemyTrophies.size();
		text.SetText("EScore: " + std::to_string(eScore));
	}
}

void EksamenGameMode::CreateSun(World* world)
{
	//Oppgave 3 
	auto* smm = world->GetStaticMeshManager();

	Entity sun = world->CreateEntity("Sun");
	sun.SetScale({ 10.f, 10.f, 10.f });
	sunEnt = sun.GetEntityId();
	auto& sunSM = sun.AddComponent<StaticMeshComponent>(). staticMeshInstanced;
	smm->LoadStaticMesh("../3Dprog22/Assets/Models/Sphere/SphereUntextured.obj", sunSM);
	sunSM.SetColor({1.f, 1.f, 0.f});
}

void EksamenGameMode::BeginPlay(World* world, entt::registry& registry)
{
	ColorTrophies(world);
	FreeTrophy(world);
}

void EksamenGameMode::EndPlay(World* world, entt::registry& registry)
{
}

void EksamenGameMode::UppdateGameStatus()
{
	if (gameStatus != NoWinner) return;

	bool bEnemy = enemyTrophies.empty();
	bool bPlayer = playerTrophies.empty();

	if (bEnemy && bPlayer)
	{
		gameStatus = Draw;
		return;
	}
	if (bEnemy)
	{
		gameStatus = AiWin;
		return;
	}
	if (bPlayer)
	{
		gameStatus = PlayerWin;
	}
}

void EksamenGameMode::InitBezier()
{
	//oppgave 7
	std::vector<glm::vec2> pts;
	pts.push_back({ 10.f, 10.f });
	pts.push_back({ 30.f, 10.f });
	pts.push_back({ 50.f, 40.f });
	pts.push_back({ 70.f, 50.f });

	bezier.SetPoints(pts);
	glm::mat4 transform(1.f);
	transform = glm::translate(transform, { -1000.f, 250.f, -500.f });
	transform = glm::scale(transform, { 20.f, 1.f, 20.f });
	bezier.SetTransform(transform);

	bezier2.SetPoints(pts);
	Transform trans;
	trans.SetPosition({ -1000.f, 300.f, 1000.f });
	trans.SetScale({ 20.f, 1.f, 20.f });
	glm::mat4 model(1.f);
	model = glm::rotate(model, glm::radians(90.f), { 0.f, 1.f, 0.f });
	trans.SetRotation(model);
	bezier2.SetTransform(trans.GetTransform());
}

void EksamenGameMode::DrawBezier()
{
	//oppgave 7
	auto* rd = RenderDebugger::Get();

	std::vector<glm::vec3> pts = bezier.GetGeneratedPoints();

	for (size_t i = 0; i < pts.size() - 1; i++)
	{
		auto& cp = pts[i];
		auto& np = pts[i + 1];

		rd->AddDebugSegment(0.f, cp, np, {1.f, 1.f, 1.f});
	}
}

void EksamenGameMode::LoadSoundFiles()
{
	explosion.LoadAudio("../3Dprog22/Assets/Audio/mi_explosion_03_hpx.mp3");
}

void EksamenGameMode::DeleteSoundFiles()
{
	explosion.CleanUp();
}

void EksamenGameMode::CreateTerrain(World* world, entt::registry& registry)
{
	world->terrainSystem.CreateTerrain(world, 
		registry, 
		"../3Dprog22/Config/Terrain/Terrain1.json");
}

void EksamenGameMode::UpdateSun(World* world, float deltatime)
{
	//Oppgave 3 
	auto* rd = RenderDebugger::Get();

	DirLight dirlight;
	world->GetDirectionalLight(dirlight);

	auto& dir = dirlight.lightDir;

	static float timer;
	float speedFactor = 0.1;
	timer += deltatime * speedFactor;

	float x = -cos(timer);
	float z = -sin(timer);
	float y = -1.f;

	world->skySystem.SetLightDirection(glm::normalize(glm::vec3( x, y, z )));
}

void EksamenGameMode::CreateTrees(World* world)
{
	/*
	auto* sm = world->GetStaticMeshManager();
	std::uniform_real_distribution<float> rf(-500.f, 500.f);
	std::default_random_engine engine;
	{
		Entity tree = world->CreateEntity("TreeInstanced");
		auto& instance = tree.AddComponent<StaticMeshInstancedComponent>().staticMeshInstanced;

		sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod0.obj", instance.LOD[0]);
		sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod1.obj", instance.LOD[1]);
		sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod2.obj", instance.LOD[2]);
		sm->LoadStaticMesh("../3Dprog22/Assets/Models/Tree2/Lod3.obj", instance.LOD[3]);

		for (size_t i = 0; i < 30; i++)
		{
			Entity treeHitbox = world->CreateEntity("TreeHB" + std::to_string(i));
			auto& treeCol = treeHitbox.AddComponent<OBBCollisionComponent>((std::uint32_t)treeHitbox.GetEntityId());
			treeCol.collisionVolume.SetExtents({ 5.f, 40.f, 5.f });
			treeCol.collisionVolume.SetLocalPos( { 0.f, 40.f, 0.f });

			Transform trans;
			while (world->collisionSystem.Intersect(&treeCol.collisionVolume))
			{
				float x = rf(engine);
				float z = rf(engine);
				float y = world->GetTerrainHeightAt(x, z);
				float ns = world->GetTerrainSlopeAt(x, z);
				if (ns < 0.4f) continue;
			
				trans.SetPosition({ x, y, z });
				int randScale = rand() % 10;
				trans.SetScale({ 20.f + randScale, 20.f + randScale, 20.f + randScale });
				glm::mat4 rot(1.f);
				rot = glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f));
				trans.SetRotation(glm::rotate(rot, glm::radians((float)i), glm::vec3(0.f, 1.f, 0.f)));

				treeHitbox.SetRotation(trans.GetRotation());
				treeHitbox.SetPosition(trans.GetPosition());
			}

			treeHitboxes.emplace_back(treeHitbox);
			instance.transforms.emplace_back(trans.GetTransform());
		}
	}
	*/
}

void EksamenGameMode::CreatePlants(World* world)
{
	constexpr float mapSize = 2700;

	auto* staticMeshManager = world->GetStaticMeshManager();
	auto& materialManager = MaterialManager::Get();

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
}

void EksamenGameMode::CreateParticles(World* world)
{
	auto* tm = world->GetTextureManager();

	Entity emitterEnt = world->CreateEntity("Particle Emitter");
	auto& emitter = emitterEnt.AddComponent<ParticleEmitterComponent>().emitter;

	tm->LoadTexture("../3Dprog22/Assets/Textures/Fire/effect_023_c_0003.jpg", emitter.particleTexture);

	emitter.lifeTime = 6.f;
	emitter.numParticles = 2000;
	emitter.pos = { 0.f, 0.f, 0.f };
	emitterEnt.SetPosition({ 0.f, 100.f, 0.f });
}

void EksamenGameMode::CreateHouse(World* world)
{
	/*
	{//creating a house   
		Entity modelEnt = world->CreateEntity("house");
		auto& model = modelEnt.AddComponent<StaticMeshComponent>().staticMeshInstanced;
		auto& transform = modelEnt.GetComponent<TransformComponent>();
		auto& collision = modelEnt.AddComponent<OBBCollisionComponent>((std::uint32_t)modelEnt.GetEntityId()).collisionVolume;

		glm::vec3 housePos = glm::vec3(-350.f, 0.f, 30.f);

		modelEnt.SetScale(glm::vec3(0.5, 0.5, 0.5));
		modelEnt.SetPosition(housePos);
		collision.SetExtents({ 60.f, 100.f, 35.f });
		collision.bStatic = true;

		world->GetStaticMeshManager()->LoadStaticMesh("./Assets/Models/Old House 2/Old House Files/Old House 2 3D Models.obj", model);

		Entity houseWall = world->CreateEntity("houseWall");
		auto& wallTransform1 = houseWall.GetComponent<TransformComponent>();
		auto& wallCollision1 = houseWall.AddComponent<OBBCollisionComponent>((std::uint32_t)houseWall.GetEntityId()).collisionVolume;
		wallCollision1.bStatic = true;

		const glm::vec3 wallOffset1 = { 70.f, 0.f, 40.f };
		houseWall.SetPosition(housePos + wallOffset1);
		wallCollision1.SetExtents({ 40.f, 20.f, 80.f });
	}
	*/
}

void EksamenGameMode::LoadAmbientSound(World* world)
{
	auto& audioManager = AudioManager::Get();

	Entity ambientEnt = world->CreateEntity("AmbientSound");
	auto& audios = ambientEnt.AddComponent<AudioComponent>().audios;

	audioManager.LoadAudio("../3Dprog22/Assets/Audio/ambientlake.mp3", audios[0]);
	audios[0].SetLooping(true);
	world->SetAmbientSound(&audios[0]);
}
