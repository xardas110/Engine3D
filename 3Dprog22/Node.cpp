#include "Node.h"

bool Node::InsertEdge(const _In_opt_ std::shared_ptr<Node>& toNode)
{
	if (!toNode)
		return false;

	edges.push_back(NodeEdge(this, toNode.get()));
	return true;
}

const std::string& Node::GetName() const
{
	return name;
}

Node::Node(const _In_ std::string& name, const float _In_  pos[3])
{
	this->name = name;
	memcpy(this->pos, pos, sizeof(pos[0]) * 3);
}

const _Ret_notnull_ std::list<NodeEdge>* Node::GetEdges() const
{
	return &edges;
}

const _Ret_notnull_ float* Node::GetPosition() const
{
	return pos;
}

void Node::ClearEdges()
{
	edges.clear();
}

void Node::SetPosition(const float _In_ newPos[3])
{
	if (newPos[0] != pos[0] && newPos[1] != pos[1] && newPos[2] != pos[2])
		return;

	for (auto i = 0; i < 3; i++)
		pos[i] = newPos[i];

}
