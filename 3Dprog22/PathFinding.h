#pragma once
#include "Path.h"
#include "Node.h"

class PathFinding
{
public:
	static bool Dijkstra(_In_opt_ Node* from, _In_opt_ Node* to, _Out_opt_ Path& path);

	static bool Dijkstra(_In_opt_ Node* from, _In_opt_ Node* to, _Out_opt_ Path& path, _Out_opt_ std::vector<const Node*>& visitedNodesContainer);

	static bool AStar(_In_opt_ Node* from, _In_opt_ Node* to, _Out_opt_ Path& path);

	static bool AStar(_In_opt_ Node* from, _In_opt_ Node* to, _Out_opt_ Path& path, _Out_opt_ std::vector<const Node*>& visitedNodesContainer, const float hOff = 0.f);

	static bool AStar(_In_opt_ Node* from, _In_opt_ Node* to, std::vector<glm::vec3>& outWayPts);
};

