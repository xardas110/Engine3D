#include "PathFindingSystem.h"
#include "World.h"
#include "RenderDebugger.h"
#include "Debug.h"

void PathFindingSystem::Init(World* world, entt::registry& registry)
{
	nodeGrid = NodeGrid::Create({ -1500.f, 0.f, -1500.f }, 200, 200, 20.f, 20.f);

	auto view = registry.view<CollisionComponent>();
	for (auto entity : view)
	{
		auto& col = view.get<CollisionComponent>(entity).col;

		if (col.bIgnorePathfinding) continue;

		nodeGrid->CheckForBlockingAABB(col.GetLocalBounds());	
	}
	
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

			if (cell.bOccupied)
			{
				rd->AddDebugBoundingBox(0.f, cell.bound.min, cell.bound.max, {1.f, 1.f, 0.f});
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

			if (cell.bOccupied)
			{
				rd->AddDebugBoundingBox(FLT_MAX, cell.bound.min, cell.bound.max, { 1.f, 0.f, 0.f });
			}
			else
			{
				rd->AddDebugBoundingBox(FLT_MAX, cell.bound.min, cell.bound.max, { 1.f, 1.f, 1.f });
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
