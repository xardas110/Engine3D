#pragma once
#include "NodeEdge.h"
#include <string>
#include <list>
#include <memory>

class Node
{
	friend class Graph;

	std::list<NodeEdge> edges;
	float pos[3]{};

public:

	Node() = delete;
	Node(const float _In_  pos[3]);

	Node(const Node& other) = delete;
	Node(Node&& other) = delete;

	Node& operator=(const Node& other) = delete;
	Node& operator=(Node&& other) = delete;

	const _Ret_notnull_ std::list<NodeEdge>* GetEdges() const;
	const _Ret_notnull_ float* GetPosition() const;

	void ClearEdges();

	void SetPosition(const float _In_ newPos[3]);
	bool InsertEdge(const _In_opt_ std::shared_ptr<Node>& toNode);
};

