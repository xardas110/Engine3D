#include "Path.h"

Path::Path(std::initializer_list<NodeEdge> edges)
{
	for (const auto& edge : edges)
	{
		this->edges.push_back(edge);
	}
}

void Path::Add(const NodeEdge& edge)
{
	this->edges.push_back(edge);
}

const NodeEdge* Path::Back() const
{
	return &this->edges.back();
}

float Path::GetLength() const
{
	float result = 0.f;

	for (const auto& edge : edges)
	{
		result += edge.GetPreCalculatedDistance();
	}

	return result;
}

_Ret_notnull_ const std::vector<NodeEdge>* Path::GetEdges() const
{
	return &edges;
}
