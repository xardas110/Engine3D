#pragma once
#include "Include/glm/glm.hpp"

class NodeEdge
{
	friend class Node;
	friend class Graph;
	friend class PathFinding;

	NodeEdge(class Node* from, class Node* to);

	class Node* from_{ nullptr };
	class Node* to_{ nullptr };
	float preCalculatedDistance{ 0.f };

public:

	NodeEdge() = delete;

	NodeEdge(const NodeEdge& other) = default;
	NodeEdge(NodeEdge&& other) noexcept = default;

	NodeEdge& operator=(const NodeEdge& other) = default;
	NodeEdge& operator=(NodeEdge&& other) noexcept = delete;

	/// <summary>
	/// Sqrt operations are very expensive in large grids, pre calculating the distance halves compute time
	/// </summary>
	float GetPreCalculatedDistance() const;

	void OnNodeMove();

	const class Node* GetFromNode() const;
	const class Node* GetToNode() const;

	/*With Y as UP!!!!*/
	glm::vec3 GetFromNodePos() const;
	glm::vec3 GetToNodePos() const;

	float CalculateSqDistance() const;
	float CalculateDistance() const;

	~NodeEdge();
};


