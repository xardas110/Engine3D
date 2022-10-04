#include "NodeGrid.h"
#include "NodeGrid.h"
#include "Node.h"
#include "RenderDebugger.h"
#include <iostream>

Cell::Cell()
	:pos({ 0.f, 0.f, 0.f })
{
}

void Cell::SetPos(const glm::vec3& pos)
{
	this->pos = pos;
}

const glm::vec3* Cell::GetPos() const
{
	return &pos;
}

NodeGrid::NodeGrid(const glm::vec3& leftBotPos, const int xNum, const int yNum, const float xScale, const float yScale)
	:xNum(xNum), yNum(yNum), xScale(xScale), yScale(yScale), leftBotPos(leftBotPos)
{
	bounds = std::make_shared<BoundingBox>();

	//bounds->SetCenter(leftBotPos + glm::vec3((float)xNum * xScale * 0.5f, (float)yNum * yScale * 0.5f, 0.f));
	//bounds->SetExtents({ (float)xNum * xScale * 0.5f, (float)yNum * yScale * 0.5f, 0.f });

	cells = new Cell * [yNum];

	for (int y = 0; y < yNum; y++)
	{
		cells[y] = new Cell[xNum];

		for (int x = 0; x < xNum; x++)
		{
			Cell cell;

			auto boundPos = leftBotPos + glm::vec3(x * xScale + (xScale * 0.5f), y * yScale + (xScale * 0.5f), 0.f);

			const auto cellNr = y * xNum + x;
			const std::string nodeName = std::to_string(cellNr);

			cell.node = std::shared_ptr<Node>(new Node(nodeName, &boundPos.x));	
			auto bb = BoundingBox();//AABBCollision(boundPos, { xScale * 0.5f, yScale * 0.5f, 0.f });
			cell.bound = std::make_shared<BoundingBox>(bb);

			cells[y][x] = cell;
		}
	}

	CalculateEdges();
}

void NodeGrid::CalculateEdges()
{
		for (int y = 0; y < yNum; y++)
		{
			for (int x = 0; x < xNum; x++)
			{
				cells[y][x].node->ClearEdges();

				if (CheckBounds(x - 1, y - 1))
					if (!cells[y - 1][x - 1].bOccupied)
						cells[y][x].node->InsertEdge(cells[y - 1][x - 1].node);

				if (CheckBounds(x + 1, y + 1))
					if (!cells[y + 1][x + 1].bOccupied)
						cells[y][x].node->InsertEdge(cells[y + 1][x + 1].node);

				if (CheckBounds(x + 1, y - 1))
					if (!cells[y - 1][x + 1].bOccupied)
						cells[y][x].node->InsertEdge(cells[y - 1][x + 1].node);


				if (CheckBounds(x - 1, y + 1))
					if (!cells[y + 1][x - 1].bOccupied)
						cells[y][x].node->InsertEdge(cells[y + 1][x - 1].node);

				if (x - 1 >= 0)
					if (!cells[y][x - 1].bOccupied)
						cells[y][x].node->InsertEdge(cells[y][x - 1].node);

				if (x + 1 < xNum)
					if (!cells[y][x + 1].bOccupied)
						cells[y][x].node->InsertEdge(cells[y][x + 1].node);

				if (y - 1 >= 0)
					if (!cells[y - 1][x].bOccupied)
						cells[y][x].node->InsertEdge(cells[y - 1][x].node);

				if (y + 1 < yNum)
					if (!cells[y + 1][x].bOccupied)
						cells[y][x].node->InsertEdge(cells[y + 1][x].node);

			}
		}
}

const glm::vec3& NodeGrid::GetLeftBotPos() const
{
	return  leftBotPos;
}

const std::shared_ptr<BoundingBox> NodeGrid::GetBounds() const
{
	return bounds;
}

_Ret_maybenull_ std::shared_ptr<Node> NodeGrid::FindNode(const _In_ std::string& name) const
{
	for (auto y = 0; y < yNum; y++)
		for (auto x = 0; x < xNum; x++)
		{
			if (cells[y][x].node->GetName() == name)
				return cells[y][x].node;
		}

	return nullptr;
}

const Cell* NodeGrid::FindCell(const int x, int y) const
{
	if (!CheckBounds(x, y))
		return nullptr;

	return &cells[y][x];
}

const Cell* NodeGrid::FindCell(const glm::vec3& worldSpace) const
{
	float pos2D[2];
	WorldToLocalGrid(worldSpace, pos2D);
	return FindCell(pos2D[0], pos2D[1]);
}

const Cell* NodeGrid::FindCellYUp(const glm::vec3& ws) const
{
	auto tempPos = ws;
	tempPos.y = tempPos.z;

	return FindCell(tempPos);
}

Cell** NodeGrid::GetCells() const
{
	return cells;
}

bool NodeGrid::CheckBounds(int x, int y) const
{
	return x >= 0 && x < xNum&& y >= 0 && y < yNum;
}

int NodeGrid::GetX() const
{
	return xNum;
}

int NodeGrid::GetY() const
{
	return yNum;
}

void NodeGrid::WorldToLocalGrid(const _In_ glm::vec3& pos, float _Inout_ posOut[2]) const
{
	posOut[0] = pos.x - leftBotPos.x;
	posOut[1] = pos.y - leftBotPos.y;
	
	posOut[0] /= xScale;
	posOut[1] /= yScale;
};

bool NodeGrid::InsertBlockingCell(int x, int y)
{
	if (!CheckBounds(x, y)) return false;

	auto& cell = cells[y][x];

	//cells[y][x].bound = std::make_shared<Bounds>(AABBCollision(*cell.GetPos(), { xScale, yScale, 0.f }));
	cells[y][x].bOccupied = true;

	CalculateEdges();

	return true;
}

bool NodeGrid::InsertBlockingCell(const _In_ glm::vec3& intersectPoint)
{
	float pos2D[2];
	WorldToLocalGrid(intersectPoint, pos2D);

	return InsertBlockingCell(pos2D[0], pos2D[1]);
}

bool NodeGrid::RemoveBlockingCell(int x, int y)
{
	if (!CheckBounds(x, y)) return false;

	auto& cell = cells[y][x];
	const auto& bound = std::make_shared<BoundingBox>();// (AABBCollision(*cell.GetPos(), { xScale, yScale, 0.f }));

	cell.bound = bound;
	cell.bOccupied = false;

	CalculateEdges();

	return true;
}

bool NodeGrid::RemoveBlockingCell(const _In_ glm::vec3& intersectPoint)
{
	float pos2D[2];
	WorldToLocalGrid(intersectPoint, pos2D);

	return RemoveBlockingCell(pos2D[0], pos2D[1]);
}

void NodeGrid::SetCellColor(int x, int y, const _In_ glm::vec3& color) const
{
	if (!CheckBounds(x, y)) return;
}

std::unique_ptr<NodeGrid> NodeGrid::Create(const glm::vec3& leftBotPos, const int xNum, const int yNum, const float xScale, const float yScale)
{
	return std::unique_ptr<NodeGrid>(new NodeGrid(leftBotPos, xNum, yNum, xScale, yScale));
}

NodeGrid::~NodeGrid()
{
	delete[] cells;
}
