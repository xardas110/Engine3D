#pragma once
#include "Include/glm/glm.hpp"
#include "TerrainConfig.h"
#include "Transform.h"
#include <vector>

//https://www.youtube.com/watch?v=z03vg2QTA8k&t=2053s&ab_channel=OREONENGINE

//TODO: Add bounding boxes and culling
class TerrainNode
{
	TerrainConfig* config;
	TerrainNode* mParent{ nullptr };
	std::vector<TerrainNode> mChildren;
	Transform localTransform;

	bool isLeaf;
	int lod; // this node lod
	glm::vec3 worldPos; // this node worldposition 
	glm::vec2 location; // this node location within the quadtree
	glm::vec2 index; // this node index within its parent
	float gap; // this node scale

public:
	TerrainNode(TerrainConfig* config, const glm::vec2& location, int lod, const glm::vec2& index);
	
	~TerrainNode();

	/*Each child node to the terrain class has its own quadtree*/
	void UpdateQuadtree();

	/*Computes this nodes world position*/
	void ComputeWorldPos();

	/*Updates level of detail based on camera position*/
	void UpdateLods();

	/*Adds a child with the desired lod*/
	void AddChildNodes(int lod);

	/*Removes all child nodes and their tree hierarchy*/
	void RemoveChildNodes();

	Transform& GetLocalTransform();

	void SetLocalTransform(const Transform& newTransform);

	void AddChild(TerrainNode& child);

	/*Gets all the leaf nodes for drawing(as patches)*/
	void GetNodes(std::vector<TerrainNode*>& outNodes);

	int GetLod() const;
	float GetGap() const;
	glm::vec3 GetWorldPos() const;
	glm::vec2 GetLocation() const;
	glm::vec2 Getindex() const;
};