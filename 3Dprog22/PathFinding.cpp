#include "PathFinding.h"
#include <queue>
#include "BinaryNode.h"


bool PathFinding::Dijkstra(_In_opt_ Node* from, _In_opt_ Node* to, _Out_opt_ Path& path)
{
	std::vector<const Node*> container;
	return Dijkstra(from, to, path, container);
}

bool PathFinding::Dijkstra(_In_opt_ Node* from, _In_opt_ Node* to, _Out_opt_ Path& path, _Out_opt_ std::vector<const Node*>& visitedNodesContainer)
{
	bool nodeFound = false;

	if (!from || !to)
		return nodeFound;

	std::priority_queue<Path, std::vector<Path>, std::greater<Path>> pq;
	std::priority_queue<Path, std::vector<Path>, std::greater<Path>> pqEnd;

	BinaryNode<const Node*> bt;

	Path startPath;
	const NodeEdge edge(from, from);

	startPath.Add(edge);
	pq.push(startPath);

	while (!pq.empty() && !nodeFound)
	{
		const auto currentPath = pq.top(); pq.pop();
		const auto* endNode = currentPath.Back()->to_;

		if (endNode == to)
			nodeFound = true;

		if (bt.Find(endNode))
			continue;

		bt.Insert(endNode);

		for (const auto& endNodeEdge : *endNode->GetEdges())
		{
			Path newPath = currentPath;
			newPath.Add(endNodeEdge);

			if (newPath.Back()->to_ == to)
				pqEnd.push(newPath);

			pq.push(newPath);
		}
	}

	if (!pqEnd.empty())
	{
		path = pqEnd.top();
	}

	bt.InOrder(visitedNodesContainer);

	return nodeFound;
}

bool PathFinding::AStar(Node* from, Node* to, Path& path)
{
	std::vector<const Node*> visitedNodesContainer;
	return AStar(from, to, path, visitedNodesContainer);
}

bool PathFinding::AStar(_In_opt_ Node* from, _In_opt_ Node* to, _Out_opt_ Path& path, _Out_opt_ std::vector<const Node*>& visitedNodesContainer, const float hOff)
{
	bool nodeFound = false;

	if (!from || !to)
		return nodeFound;

	static const auto f = [](const std::pair<float, Path>& a, const std::pair<float, Path>& b)
	{
		return (a.first + a.second.GetLength()) > (b.first + b.second.GetLength());
	};

	std::priority_queue<std::pair<float, Path>, std::vector<std::pair<float, Path>>, decltype(f)> pq(f);
	BinaryNode<const Node*> bt;

	const NodeEdge edge(from, from);
	Path newPath;

	newPath.Add(edge);
	pq.push({ 0.f, newPath });

	while (!pq.empty() && !nodeFound)
	{
		const auto currentPath = pq.top(); pq.pop();
		const auto* endNode = currentPath.second.Back()->to_;

		if (endNode == to)
		{
			path = currentPath.second;
			nodeFound = true;
			break;
		}

		if (bt.Find(endNode))
			continue;

		bt.Insert(endNode);

		const auto edges = *endNode->GetEdges();
		for (const auto& endNodeEdge : edges)
		{
			newPath = currentPath.second;

			{
				newPath.Add(endNodeEdge);

				const auto* newPathTo = newPath.Back()->to_->GetPosition();
				const float dirToEnd[2] = { to->GetPosition()[0] - newPathTo[0], to->GetPosition()[1] - newPathTo[1] };
				const float manhattanLength = abs(dirToEnd[0]) + abs(dirToEnd[1]) + hOff;

				pq.push({ manhattanLength, newPath });
			}
		}
	}

	bt.InOrder(visitedNodesContainer);

	return nodeFound;
}

bool PathFinding::AStar(Node* from, Node* to, std::vector<glm::vec3>& outWayPts)
{
	Path path;
	if (PathFinding::AStar(from, to, path))
	{
		for (const auto& edge : *path.GetEdges())
		{
			outWayPts.emplace_back(edge.GetToNodePos());
		}
		return true;
	}

	return false;
}
