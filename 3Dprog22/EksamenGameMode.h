#pragma once
#include "GameMode.h"
#include "BezierFunction.h"
#include <utility>
#include "AudioData.h"
#include <string>

class Entity;

enum GameStatus
{
	NoWinner,
	PlayerWin,
	Draw,
	AiWin
};

/// <summary>
/// Prøver å holde alle eksamen funksjonen i klassen her som er relatert til all gameplay
/// </summary>
class EksamenGameMode : public GameMode
{
	~EksamenGameMode();

    virtual void Create(class World* world, entt::registry& registry) override;

	virtual void BeginPlay(class World* world, entt::registry& registry) override;

	virtual void EndPlay(class World* world, entt::registry& registry) override;

	//Funksjon som kun oppdateres i play modus
	virtual void Update(class World* world, entt::registry& registry, float deltatime) override;

	/*Deferred deletion*/
	void UpdateEntForDeletion(class World* world);

	//Funksjon som updater during editor
	virtual void UpdateEditor(class World* world, entt::registry& registry, float deltatime) override;

	//Oppgave 14, alle ents slettes utenom de som skal være dær by default
	virtual void Reset(class World* world, entt::registry& registry) override;

	//Oppgave 11 og 12
	void UppdateGameStatus();

	// oppgave 7
	void InitBezier();
	// oppgave 7
	void DrawBezier();
private:

	void LoadSoundFiles();
	void DeleteSoundFiles();

	void CreateTerrain(class World* world, entt::registry& registry);

	//oppgave 4
	void CreatePlayer(class World* world, entt::registry& registry);
	void UpdatePlayer(class World* world, float deltatime);

	//Oppgave 7
	void CreateBomber(class World* world, entt::registry& registry);
	void CreateBomber2(class World* world, entt::registry& registry);
	//Oppgave 7
	void UpdateBomber(class World* world, entt::registry& registry, float deltatime);
	void UpdateBomber2(class World* world, entt::registry& registry, float deltatime);

	//Oppgave 7
	void DropBomb(class World* world, const glm::vec3& bomberPos);
	//oppgave 7
	void UpdateDroppedBombs(class World* world, float deltatime);

	//Oppgave 16 mer refactor og gameplay
	void BombStage1(Entity& bomb, float deltatime);
	void BombStage2(Entity& bomb, float deltatime);
	void BombStage3(Entity& bomb, float deltatime);
	void ExplodeBomb(class World* world, Entity& bomb, float deltatime);

	//oppgave 9 ai character
	void CreateAiCharacter(class World* world, entt::registry& registry);
	//oppgave 9
	void UpdateAiCharacter(class World* world, float deltatime);

	//Oppgave 8 re-upload
	void CreateTophies(class World* world);
	//Oppgave 8 Setter farge på trophies når spillet starter, sånn at spiller ikke jukser med å se fargene på forhand i editor modus
	void ColorTrophies(class World* world); //Oppgave 8 re-upload
	// oppgave 8
	void UpdateTrophies(class World* world);

	void FreeTrophy(class World* world);

	//Oppgave 11
    void CreateStatusBillboard(class World* world);
	//Oppgave 11
	void UpdateStatusBillboard(class World* world, float deltatime);
	//Oppgave 3 
	void CreateSun(class World* world);
	//Oppgave 3 
	void UpdateSun(class World* world, float deltatime);

	//Pynt
	void CreateObjects(class World* world, std::array<std::string, 4> paths, int num = 200, float randBegin = 1.f, float randEnd = 1.f);
	void CreatePlants(class World* world);
	void CreateExplosionParticles(class World* world, const glm::vec3& pos);
	void LoadAmbientSound(class World* world);

	entt::entity sunEnt;
	entt::entity playerEnt;
	entt::entity aiEnt;
	entt::entity bomberEnt;
	entt::entity bomberEnt2;
	entt::entity statusBillboardEnt;
	entt::entity playerScoreEnt;
	entt::entity enemyScoreEnt;

	std::vector<Entity> playerTrophies;
	std::vector<Entity> enemyTrophies;

	std::vector<entt::entity> entForDeletion;

	AudioData explosion;

	//oppgave 7
	std::vector<std::pair<float, Entity>> bombs;

	//Oppgave10
	std::vector<Entity> barricades;
	std::vector<Entity> treeHitboxes;

	//oppgave 7
	BezierFunction bezier;
	BezierFunction bezier2;

	//Oppgave 11 og 12
	GameStatus gameStatus{ GameStatus::NoWinner };
};

