#include "PathFindingSystem.h"
#include "World.h"
#include "RenderDebugger.h"
#include "Debug.h"

void PathFindingSystem::Init(World* world, entt::registry& registry)
{
	nodeGrid = NodeGrid::Create({ -1000.f, -1000.f, 0 }, 100, 100, 20.f, 20.f);

	/*
	auto view = registry.view<OBBCollisionComponent>();

	for (auto entity : view)
	{
		auto& col = view.get<OBBCollisionComponent>(entity);

		//if (col.collisionVolume.bIgnorePathFinding) continue;

		//nodeGrid->CheckForBlockingOBB(&col.collisionVolume);
	}
	*/
	nodeGrid->CalculateEdges();
}

void PathFindingSystem::Update(World* world, entt::registry& registry)
{
}

void PathFindingSystem::DrawDebugPF1()
{
	auto* rd = RenderDebugger::Get();
	auto cells = nodeGrid->GetCells();

	for (size_t y = 0; y < nodeGrid->GetY(); y++)
	{
		for (size_t x = 0; x < nodeGrid->GetX(); x++)
		{
			auto& cell = cells[y][x];

			auto tempPos = glm::vec3(0);// cell.bound->GetCenterOfMass();
			tempPos.z = tempPos.y;
			tempPos.y = 0.f;

			auto tempScale = glm::vec3(1); // cell.bound->e;
			tempScale.z = tempScale.y;
			tempScale.y = 0.f;

			if (cell.bOccupied)
			{
				rd->AddDebugBox(0.f, tempPos, tempScale, glm::identity<glm::quat>(), {1.f, 1.f, 0.f});
			}
		}
	}
}

void PathFindingSystem::DrawDebugPF2()
{
	auto* rd = RenderDebugger::Get();
	auto cells = nodeGrid->GetCells();

	for (size_t y = 0; y < nodeGrid->GetY(); y++)
	{
		for (size_t x = 0; x < nodeGrid->GetX(); x++)
		{
			auto& cell = cells[y][x];

			auto tempPos = glm::vec3(0.f);//cell.bound->GetCenterOfMass();
			tempPos.z = tempPos.y;
			tempPos.y = 0.f;

			auto tempScale = glm::vec3(0.f);//cell.bound->e;
			tempScale.z = tempScale.y;
			tempScale.y = 0.f;

			if (cell.bOccupied)
			{
				rd->AddDebugBox(FLT_MAX, tempPos, tempScale, glm::identity<glm::quat>());
			}
			else
			{
				rd->AddDebugBox(FLT_MAX, tempPos, tempScale, glm::identity<glm::quat>(), {1.f, 1.f, 1.f});
			}
		}
	}
}

Node* PathFindingSystem::GetNode(const glm::vec3& ws)
{
	if (auto* cell = nodeGrid->FindCellYUp(ws))
		return nodeGrid->FindCellYUp(ws)->node.get();
	else
	{
		return nullptr;
	}
}

bool PathFindingSystem::IsOccupied(const glm::vec3& ws)
{
	auto* cell = nodeGrid->FindCellYUp(ws);
	if (!cell) return false;

	return cell->bOccupied;
}
