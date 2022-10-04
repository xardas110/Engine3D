#pragma once

#include <vector>
#include "BoundingVolumes.h"
#include "Include/entt/entt.hpp"
#include "ContactManifold.h"
#include "Include/ImGui/imgui.h"
#include <mutex>


#define MAX_POINTS_PR_QUAD 20
#define OCTREE_MAX_DEPTH 6

struct OctreeData
{
	entt::entity mEntity{entt::null};
	BoundingBox mBounds;
	bool bStatic = false;
};

class Octree
{
	enum Coord
	{
		NW, NE, SE, SW, NWB, NEB, SEB, SWB, Size
	};

	int					currentDepth{ 0 };

	/*Clears this nodes volumes.*/
	void				ClearVolumes();

	bool				HasChildren() const;

	bool				HasReachedMaxDepth() const;

	/*Checks a cardinal point for intersection, if it intersects then the aabb will be returned in the out param
	 * The function is used to indicate if a new octree should be built
	 */
	bool				CheckCardinalDirection(Coord cd, BoundingBox& volume, BoundingBox& outAABB);

	/*Pushes a volume down the tree*/
	void				PushDown(OctreeData& volume, BoundingBox& outBoundsCheckContainer);

	/*The nodes below will maybe get deleted and pushed up*/
	void				PushUp();

	/*Used only when a new node has to be created*/
	Octree*				CreateNewNode(BoundingBox& boundsInfo) const;

public:
	Octree				(const BoundingBox& rect);

	~Octree();

	[[nodiscard]]
	bool				IsLeaf() const;

	void				FindCollisionPairs(std::vector<CollisionPair>& collisionPair);

	void				Insert(OctreeData& poly);

	void			    Remove(OctreeData& volume, int& timesRemoved);

	void				CountChildrenVolumes(size_t& outCount);

	void				CollapseChildren();

	const				BoundingBox& GetBounds() const;

	void				GetAllBounds(std::vector<BoundingBox>& container) const;

	void				GetAllLeafBounds(std::vector<BoundingBox>& container) const;

	void				GetAllNonLeafBounds(std::vector<BoundingBox>& container) const;

	void				GetOctreeLeafs(std::vector<Octree*>& outOcts);

	void				DrawLeafBounds() const;

private:
	BoundingBox bounds;
	std::vector<OctreeData> polygons;

	std::mutex mutexLock;

	Octree* nodes[Size]{ nullptr }; //Faster than a vector, the size is always known to be 8
};

