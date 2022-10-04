#pragma once
#include "NodeEdge.h"
#include <vector>

class Path
{
	std::vector<NodeEdge> edges;

public:
	Path() = default;

	Path(std::initializer_list<NodeEdge> edges);

	void  Add(const NodeEdge& edge);

	const NodeEdge* Back() const;

	float GetLength() const;

	_Ret_notnull_ const std::vector<NodeEdge>* GetEdges() const;

	bool operator > (const Path& other) const { return GetLength() > other.GetLength(); }
};
