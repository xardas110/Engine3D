#pragma once
#include <memory>
#include "Node.h"
#include "BoundingVolumes.h"

struct Cell
{
	Cell();

	Cell(const Cell&) = default;
	Cell(Cell&&) = default;

	Cell& operator= (const Cell&) = default;
	Cell& operator= (Cell&&) = default;

	~Cell() = default;

	std::shared_ptr<Node>   node;
	BoundingBox	bound;

	void SetPos(const glm::vec3&);
	const glm::vec3* GetPos() const;

	bool bOccupied = false;

private:
	glm::vec3 pos;
};

class NodeGrid
{
	friend class PathFindingSystem;

	NodeGrid(const glm::vec3& leftBotPos, const int xNum, const int yNum, const float xScale, const float yScale);

	const int xNum;
	const int yNum;
	const float xScale;
	const float yScale;

	const glm::vec3 leftBotPos;

	BoundingBox bounds;

	Cell** cells;
	void CalculateEdges();
public:
	
	const _Ret_notnull_ glm::vec3& GetLeftBotPos() const;
	const _Ret_notnull_ BoundingBox GetBounds() const;

	NodeGrid() = delete;

	NodeGrid(const NodeGrid&) = delete;
	NodeGrid(NodeGrid&&) = delete;

	NodeGrid& operator= (const NodeGrid&) = delete;
	NodeGrid& operator= (NodeGrid&&) = delete;

	//localspace
	const Cell* _Ret_maybenull_ FindCell(const _In_ int x, int y) const;
	//worldspace
	const Cell* _Ret_maybenull_ FindCell(const _In_ glm::vec3& worldSpace) const;

	const Cell* _Ret_maybenull_ FindCellYUp(const _In_ glm::vec3& ws) const;

	Cell** GetCells() const;

	bool CheckBounds(int x, int y) const;

	/// <summary>
	/// Num of x cells
	int GetX() const;

	/// <summary>
	/// Num of y cells
	int GetY() const;

	void WorldToLocalGrid(const _In_ glm::vec3& pos, float _Inout_ posOut[2]) const;

	//localspace
	bool InsertBlockingCell(int x, int y);

	//worldspace
	bool InsertBlockingCell(const _In_ glm::vec3& intersectPoint);

	bool RemoveBlockingCell(int x, int y);

	bool RemoveBlockingCell(const _In_ glm::vec3& intersectPoint);

	void SetCellColor(int x, int y, const _In_ glm::vec3& color) const;

	static std::unique_ptr<NodeGrid> Create(const _In_ glm::vec3& leftBotPos, const int xNum, const int yNum, const float xScale, const float yScale);

	void CheckForBlockingAABB(const BoundingBox& other);

	~NodeGrid();
};

