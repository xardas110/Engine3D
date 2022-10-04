#include "Octree.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include "RenderDebugger.h"

Octree::Octree(const BoundingBox& rect)
	: bounds(rect) {
	polygons.reserve(MAX_POINTS_PR_QUAD + 1);
}

Octree::~Octree()
{
	for (auto i = 0; i < Size; i++)
	{
		if (!nodes[i]) continue;

		nodes[i]->~Octree(); // not needed
		delete nodes[i];
		nodes[i] = nullptr;
	}
};

bool Octree::IsLeaf() const
{
	for (size_t i = 0; i < Size; i++)
	{
		if (nodes[i]) return false;
	}
	return true;
}

void Octree::FindCollisionPairs(std::vector<CollisionPair>& collisionPair)
{
	if (IsLeaf())
	{
		for (int i = 0; i < polygons.size(); i++)
			for (auto j = i + 1; j < polygons.size(); j++)
			{ 
				if (polygons[i].bStatic && polygons[j].bStatic) continue;
				if (polygons[i].mBounds.Intersect(polygons[j].mBounds))
				{ 
					bool bExist = false;

					CollisionPair pair{ polygons[i].mEntity, polygons[j].mEntity };

					for (const auto& p : collisionPair)
					{
						if (p == pair)
						{
							bExist = true;
							break;
						}
					}

					if (!bExist) collisionPair.push_back({ polygons[i].mEntity, polygons[j].mEntity });				
				}
			}
	}

	for (size_t i = 0; i < Size; i++)
	{
		if (!nodes[i]) continue;

		nodes[i]->FindCollisionPairs(collisionPair);
	}
}

void Octree::PushDown(OctreeData& volume, BoundingBox& outBoundsCheckContainer)
{
	for (auto i = 0; i < Size; i++)
	{
		if (CheckCardinalDirection((Coord)i, volume.mBounds, outBoundsCheckContainer))
		{
			if (!nodes[i]) 
			{ 
				nodes[i] = CreateNewNode(outBoundsCheckContainer);
			}				
			nodes[i]->Insert(volume);
		}
	}
}

void Octree::PushUp()
{
	if (IsLeaf()) return; //A leaf has no children.

	size_t childrenVolumeSize{ 0 };
	CountChildrenVolumes(childrenVolumeSize);

	//If no child has volumes, collapse! An early exit.	
	if (childrenVolumeSize == 0)
	{
		CollapseChildren();
		return;
	}

	//Insert children volumes if there is space.
	const auto totalPolygons = childrenVolumeSize + polygons.size();
	if (totalPolygons <= MAX_POINTS_PR_QUAD)
	{
		for (auto i = 0; i < Size; i++)
		{
			if (!nodes[i]) continue;

			polygons.insert(polygons.end(), nodes[i]->polygons.begin(), nodes[i]->polygons.end());
		}

		CollapseChildren();
	}
	else //There might not be any space in this node to hold all the children, at least delete empty nodes!
	{
		for (auto i = 0; i < Size; i++)
		{
			if (!nodes[i]) continue;

			if (nodes[i]->IsLeaf() && !nodes[i]->HasChildren())
			{
				delete nodes[i];
				nodes[i] = nullptr;
			}
		}
	}
}

void Octree::Insert(OctreeData& poly)
{
	if (!poly.mBounds.Intersect(bounds))
		return;
	
	BoundingBox boundsCheck;// Container for CheckCardinalDirection, performance improvement by passing it as ref. The container is used to hold information about the oct that needs creation.

	if (!IsLeaf())
	{
		PushDown(poly, boundsCheck);
		return;
	}
	
	mutexLock.lock();
	polygons.push_back(poly); //In case of duplicates.

	if (polygons.size() > MAX_POINTS_PR_QUAD && !HasReachedMaxDepth())
	{	
		for (auto& existingVolumes : polygons)
		{
			PushDown(existingVolumes, boundsCheck);
		}
		ClearVolumes();
	}
	mutexLock.unlock();
}

void Octree::Remove(OctreeData& volume, int& timesRemoved)
{
	if (!bounds.Intersect(volume.mBounds)) return;

	if (IsLeaf())
	{
		for (auto it = polygons.begin(); it != polygons.end();)
		{
			if ((*it).mEntity != volume.mEntity) { ++it; continue; }

			it = polygons.erase(it); timesRemoved++;
		}
	}
	else
	{
		for (auto i = 0; i < Coord::Size; i++)
		{
			if (!nodes[i]) continue;

			nodes[i]->Remove(volume, timesRemoved);
		}
	}
	PushUp();
}

Octree* Octree::CreateNewNode(BoundingBox& boundsInfo) const
{
	auto* node = new Octree(boundsInfo);
	node->currentDepth = currentDepth + 1;
	return node;
}

void Octree::GetOctreeLeafs(std::vector<Octree*>& outOcts)
{
	if (IsLeaf())
	{
		outOcts.emplace_back(this);
	}
	else
	{
		for (size_t i = 0; i < Size; i++)
		{
			if (nodes[i])
			{
				nodes[i]->GetOctreeLeafs(outOcts);
			}
		}
	}
}

void Octree::CountChildrenVolumes(size_t& outCount)
{
	for (auto i = 0; i < Size; i++)
	{
		if (!nodes[i]) continue;

		outCount += nodes[i]->polygons.size();
		nodes[i]->CountChildrenVolumes(outCount);
	}
}

void Octree::CollapseChildren()
{
	for (auto i = 0; i < Size; i++)
	{
		if (!nodes[i]) continue;

		//If the children don't have nodes by any case. Should not be possible if ran correctly.
		//Better to be safe anyways, or it will have lose pointers.
		nodes[i]->CollapseChildren();

		delete nodes[i];
		nodes[i] = nullptr;
	}
}

const BoundingBox& Octree::GetBounds() const
{
	return bounds;
}

void Octree::ClearVolumes()
{
	polygons.clear();
	//polygons.shrink_to_fit(); If memory saving is needed
}

bool Octree::HasChildren() const
{
	return !polygons.empty();
}

bool Octree::HasReachedMaxDepth() const
{
	return currentDepth > OCTREE_MAX_DEPTH;
}

void Octree::GetAllBounds(std::vector<BoundingBox>& container) const
{
	container.emplace_back(bounds);

	for (auto i = 0; i < Size; i++)
	{
		if (nodes[i])
			nodes[i]->GetAllBounds(container);
	}
}

void Octree::GetAllLeafBounds(std::vector<BoundingBox>& container) const
{
	if (IsLeaf())
		container.emplace_back(bounds);

	for (auto i = 0; i < Size; i++)
	{
		if (nodes[i])
			nodes[i]->GetAllLeafBounds(container);
	}
}

void Octree::GetAllNonLeafBounds(std::vector<BoundingBox>& container) const
{
	if (!IsLeaf())
		container.emplace_back(bounds);

	for (auto i = 0; i < Size; i++)
	{
		if (nodes[i])
			nodes[i]->GetAllLeafBounds(container);
	}
}

void Octree::DrawLeafBounds() const
{
	auto* rd = RenderDebugger::Get();

	std::vector<BoundingBox> bbs;
	GetAllLeafBounds(bbs);

	for (auto& bounds : bbs)
	{
		rd->AddDebugBoundingBox(0.f, bounds.min, bounds.max);
	}
}

bool Octree::CheckCardinalDirection(Coord cd, BoundingBox& volume, BoundingBox& outAABB)
{
	/*Should be fairly optimized, but there could be more optimizations done. Only if needed*/
	
	const glm::vec3& min = bounds.min;
	const glm::vec3& max = bounds.max;
	const auto extent = (max - min) * 0.5f;
	const glm::vec3 center = bounds.min + ((max - min) * 0.5f);
	
	switch (cd)
	{
		case NW:
		{
			outAABB.min.x = min.x;
			outAABB.min.y = min.y + extent.y;
			outAABB.min.z = min.z;

			outAABB.max.x = center.x;
			outAABB.max.y = center.y + extent.y;
			outAABB.max.z = center.z;

			return volume.Intersect(outAABB);
		}
		case NE:
		{
			outAABB.min.x = center.x;
			outAABB.min.y = center.y;
			outAABB.min.z = center.z - extent.z;

			outAABB.max.x = max.x;
			outAABB.max.y = max.y;
			outAABB.max.z = max.z - extent.z;

			return volume.Intersect(outAABB);
		}
		case SW:
		{
			outAABB.min = min;
			outAABB.max = center;

			return volume.Intersect(outAABB);
		}
		case SE:
		{
			outAABB.min.x = min.x + extent.x;
			outAABB.min.y = min.y;
			outAABB.min.z = min.z;

			outAABB.max.x = center.x + extent.x;
			outAABB.max.y = center.y;
			outAABB.max.z = center.z;

			return volume.Intersect(outAABB);
		}
		case NWB:
		{
			outAABB.min.x = center.x - extent.x;
			outAABB.min.y = center.y;
			outAABB.min.z = center.z;

			outAABB.max.x = max.x - extent.x;
			outAABB.max.y = max.y;
			outAABB.max.z = max.z;

			return volume.Intersect(outAABB);
		}
		case NEB:
		{
			outAABB.min.x = center.x;
			outAABB.min.y = center.y;
			outAABB.min.z = center.z;

			outAABB.max.x = max.x;
			outAABB.max.y = max.y;
			outAABB.max.z = max.z;

			return volume.Intersect(outAABB);
		}
		case SWB:
		{
			outAABB.min.x = min.x;
			outAABB.min.y = min.y;
			outAABB.min.z = min.z + extent.z;

			outAABB.max.x = center.x;
			outAABB.max.y = center.y;
			outAABB.max.z = center.z + extent.z;

			return volume.Intersect(outAABB);
		}
		case SEB:
		{
			outAABB.min.x = center.x;
			outAABB.min.y = center.y - extent.z;
			outAABB.min.z = center.z;

			outAABB.max.x = max.x;
			outAABB.max.y = max.y - extent.y;
			outAABB.max.z = max.z;

			return volume.Intersect(outAABB);
		}
		default:
		{
			std::cout << "default inside octree is running" << std::endl;
			return false;
		}
	}
}

