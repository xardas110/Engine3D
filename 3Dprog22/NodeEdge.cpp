#include "NodeEdge.h"
#include "Node.h"

NodeEdge::NodeEdge(Node* from, Node* to)
	: from_(from), to_(to)
{
	preCalculatedDistance = CalculateDistance();
}

NodeEdge::~NodeEdge()
{

}

float NodeEdge::GetPreCalculatedDistance() const
{
	return preCalculatedDistance;
}

void NodeEdge::OnNodeMove()
{
	preCalculatedDistance = CalculateDistance();
}

const Node* NodeEdge::GetFromNode() const
{
	return from_;
}

const Node* NodeEdge::GetToNode() const
{
	return to_;
}

glm::vec3 NodeEdge::GetFromNodePos() const
{
	glm::vec3 result;
	memcpy(&result, from_->GetPosition(), 12);
	return result;
}

glm::vec3 NodeEdge::GetToNodePos() const
{
	glm::vec3 result;
	memcpy(&result, to_->GetPosition(), 12);
	return result;
}

float NodeEdge::CalculateSqDistance() const
{
	const float* A = from_->GetPosition();
	const float* B = to_->GetPosition();

	float result{};
	float dir[3]{};

	for (auto i = 0; i < 3; i++)
	{
		dir[i] = B[i] - A[i];
		result += dir[i] * dir[i];
	}
	return result;
}

float NodeEdge::CalculateDistance() const
{
	return sqrtf(CalculateSqDistance());
}

