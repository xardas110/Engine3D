#pragma once

#include "Include/entt/entt.hpp"
#include "Camera.h"
#include "skybox.h"
#include <QOpenGLFunctions_4_1_Core>
#include <map>
#include <qevent.h>

#include "Components.h"
#include "Octree.h"
#include "texturemanager.h"
#include "MeshManager.h"
#include "shader.h"
#include "WorldSerializer.h"
#include "SkeletalMesh.h"
#include "SkeletalMeshManager.h"
#include "StaticMeshManager.h"
#include "ShaderManager.h"
#include "FrameBufferManager.h"
#include "UBOManager.h"
#include "Modules.h"
#include "TerrainSystem.h"
#include "StaticMeshSystem.h"
#include "AlphaPassSystem.h"
#include "SkeletalMeshSystem.h"
#include "SkySystem.h"
#include "AudioSystem.h"
#include "StaticMeshInstancedSystem.h"
#include "VegetationSystem.h"
#include "ParticleSystem.h"
#include "CameraSystem.h"
#include "BillboardSystem.h"
#include "GameMode.h"
#include "EksamenGameMode.h"
#include "PathFindingSystem.h"
#include "DeferredRendererConfig.h"
#include "PhysicsSystem.h"
#include "BSplineSystem.h"

constexpr int GRAVITY_DEFAULT = -9.87f;

struct WorldSettings
{
	bool bDrawDynamicOctree{ false };
	bool bDrawOctreeLeafs{ false };
	bool bDrawOctreeNonLeafs{ false };
	bool bDrawDebugBoxes{ false };
	bool bDrawCameraFrustumCorners{ false };
	bool bShowPathfindingOccupied{ false };
	bool bShowPathfinding{ false };
	bool bShowWorldAxises{ false };

};

namespace WorldState
{
	enum State
	{
		Editor,
		Play
	};
}

class Entity;
class World
{
	friend class Entity;
	friend class Editor;
	friend class WorldSerializer;
	friend class RenderSystem;
	friend class CollisionManager;
	friend class SkeletalMesh;
	friend class MeshManager;
	friend class RenderWindow;
	friend struct CascadedShadowMaps;
	friend class DeferredRenderer;
	friend class QObject;

	entt::registry		entRegistry;
	
	Camera*				renderCamera{ nullptr };
	TextureManager*		textureManager{nullptr};
	MeshManager*		meshManager{nullptr};
	StaticMeshManager	staticMeshManager;
	SkeletalMeshManager	skeletalMeshManager;
	class DeferredRenderer* deferedRenderer{ nullptr };
public:

	TerrainSystem		terrainSystem;
	StaticMeshSystem	staticMeshSystem;
	SkeletalMeshSystem  skeletalMeshSystem;
	StaticMeshInstancedSystem staticMeshInstancedSystem;
	VegetationSystem	vegetationSystem;
	AlphaPassSystem		alphaPassSystem;
	SkySystem			skySystem;
	AudioSystem			audioSystem;
	ParticleSystem		particleSystem;
	CameraSystem		cameraSystem;
	BillboardSystem		billboardSystem;
	PathFindingSystem	pathFindingSystem;
	GameMode*			gameMode{ nullptr };
	PhysicsSystem*		physicsSystem{ nullptr };
	BSplineSystem		bSplineSystem;
private:
	
	WorldSerializer		worldSerializer;
	
	WorldSettings		worldSettings;

	float				deltatime{0.f};

	void UpdateMousePressedScripts(QMouseEvent* event);
	void UpdateMouseReleasedScripts(QMouseEvent* event);
	void UpdateMouseMoveScripts(QMouseEvent* event);
	void UpdateMouseHeldScripts(std::map<int, bool>* mouseHeld, float deltaTime);

	void UpdateKeyPressedScripts(QKeyEvent* event);
	void UpdateKeyReleasedScripts(QKeyEvent* event);
	void UpdateKeyHeldScripts(std::map<int, bool>* keyHeld, float deltaTime);

	void InitScripts();
	void DestroyScripts();
	
	void InitBeginPlayScripts();
	void UpdateCameraScripts(float deltaTime);
	void UpdateScriptComponents(float deltaTime);
	void UpdateScripts(float deltaTime);

	void OnBeginPlay();
	void OnEndPlay();

	void UpdateDeferredEntityDeletion();

	void DrawDebugSettings(float deltatime);

	WorldState::State worldState = WorldState::Editor;

	std::vector<entt::entity> entityForDeletion;

	void SetState(WorldState::State newState);
	
public:
	World();
	~World();
	/*Should be private TODO:make private, but qeditor needs access to this*/
	DeferredRendererConfig& GetRenderConfig();
	TerrainConfig* GetTerrainConfig(); //So editor can change stuff
	Terrain* GetTerrain(); //for editor

	[[nodiscard]] WorldState::State GetState() const;
	bool IsState(WorldState::State state) const;
	bool IsNotState(WorldState::State state) const;

	Entity CreateEntity(const std::string& tag);
	
	void UpdateWorldAxises();

	void OnInit(Camera* renderCamera);
	void OnUpdate(float deltaTime);
	void OnRender(float deltaTime);

	void LoadRenderConfig(const std::string& jsonPath);

	bool GetDirectionalLight(DirLight& outDirlight);

	void DeleteEntity(entt::entity id);

	void DeleteEntities(std::initializer_list<std::string> stringFlags);

	Camera* GetRenderCamera() const;
	
	void OnMousePressedFromRenderWindow(QMouseEvent* event);
	void OnMouseReleasedFromRenderWindow(QMouseEvent* event);
	void OnMouseMoveFromRenderWindow(QMouseEvent* event);
	void OnMouseButtonHeldFromRenderWindow(std::map<int, bool>* mouseHeld, float deltaTime);

	void OnExposeEvent(QExposeEvent* e);

	void OnKeyPressedFromRenderWindow(QKeyEvent* event);
	void OnKeyReleasedFromRenderWindow(QKeyEvent* event);
	void OnKeysHeld(std::map<int, bool>* keysHeld, float deltaTime);
	
    void SetRenderCamera(Camera* renderCamera);

	void SetAmbientSound(AudioData* audio);
	AudioData* GetAmbientSound();

	float GetTerrainHeightAt(float x, float z);
	float GetTerrainSlopeAt(float x, float z);
	glm::vec3 GetTerrainNormalAt(float x, float z);

	static float GetElapsedTimeInSeconds();

	SkeletalMeshManager*	GetSkeletalMeshManager();
	StaticMeshManager*		GetStaticMeshManager();
	MeshManager*			GetMeshManager();	
	TextureManager*			GetTextureManager();

	const WorldSettings&	GetWorldSettings() const;

	void PreLoadAssets(const std::string& jsonPath);

	void SetGameMode(GameMode* gm);
	void StartGameMode(int index);

    void on_saveButton_clicked();
    void on_loadButton_clicked();
	
	void SaveWorld(const std::string& path);
	void LoadWorld(const std::string& path);

	entt::entity CreateTerrain(const std::string& configJsonPath);
	entt::entity CreateTerrain(Texture& heightmap, const std::string& configJsonPath);

	static size_t nameVal;
	static size_t GetNextNameVal();

	static float GetDeltaTime();

	static glm::vec3 GetWorldUp();

	//singleton for now
	static World* Get();

	static glm::vec3 GetGravity();

protected:
	std::vector<Entity> entities;
	entt::entity playerEnt;
	AudioData* ambientSound;

	/*Virtual create, begin, update*/
	virtual void Create() {};
	virtual void BeginPlay() {};
	virtual void EndPlay() { };
	virtual void Update(float deltaTime) {};

	virtual void KeyPressed(QKeyEvent* event) {};
	virtual void KeyReleased(QKeyEvent* event) {};

	virtual void MousePressed(QMouseEvent* event) {};
	virtual void MouseMoved(QMouseEvent* event) {};
	virtual void MouseReleased(QMouseEvent* event) {};
};

