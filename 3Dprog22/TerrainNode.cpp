#include "TerrainNode.h"
#include "World.h"

TerrainNode::~TerrainNode()
{
}

TerrainNode::TerrainNode(TerrainConfig* config, const glm::vec2& location, int lod, const glm::vec2& index)
{
	this->config = config;
	this->isLeaf = true;
	this->index = index;
	this->lod = lod;
	this->location = location;
	this->gap = 1.f / ((float)Terrain::gridSize * powf(2.f, (float)lod));

	glm::vec3 localScaling(gap, 0.f, gap);
	glm::vec3 localTranslation(location.x, 0.f, location.y);

	GetLocalTransform().SetScale(localScaling);
	GetLocalTransform().SetPosition(localTranslation);

	ComputeWorldPos();
	UpdateQuadtree();
}

void TerrainNode::UpdateQuadtree()
{
	ComputeWorldPos();
	UpdateLods();

	for (TerrainNode& child : mChildren)
		child.UpdateQuadtree();
}

void TerrainNode::AddChildNodes(int lod)
{
	if (isLeaf)
	{
		isLeaf = false;
	}

	if (mChildren.empty())
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				float gapX = (float)i * gap * 0.5f;
				float gapY = (float)j * gap * 0.5f;
				glm::vec2 gap = glm::vec2(gapX, gapY);
				auto terrainNode = TerrainNode(config, this->location + gap, lod, glm::vec2(i, j));
				AddChild(terrainNode);
			}
		}
	}	
}

void TerrainNode::RemoveChildNodes()
{
	if (!isLeaf)
	{
		isLeaf = true;
	}

	if (!mChildren.empty())
	{
		mChildren.clear();
	}
}

void TerrainNode::UpdateLods()
{
	auto* world = World::Get(); //TODO: should not know about the camera class, send in camera pos
	auto* camera = world->GetRenderCamera();

	float distance = glm::length(camera->GetPosition() - worldPos);

	if (distance < config->lod_range[lod])
	{
		AddChildNodes(lod + 1);
	}
	else if (distance >= config->lod_range[lod])
	{
		RemoveChildNodes();
	}
}

void TerrainNode::ComputeWorldPos()
{
	auto* world = World::Get();
	auto* camera = world->GetRenderCamera();

	if (camera->GetPosition().y > config->scaleY)
	{
		worldPos.y = config->scaleY;
	}
	else
	{
		worldPos.y = camera->GetPosition().y;
	}

	glm::vec2 a1 = location + gap * 0.5f;
	glm::vec2 a2 = a1 * config->scaleXZ;
	glm::vec2 a3 = a2 - config->scaleXZ * 0.5f;

	worldPos.x = a3.x;
	worldPos.z = a3.y;
}

Transform& TerrainNode::GetLocalTransform()
{
	return localTransform;
}

void TerrainNode::SetLocalTransform(const Transform& newTransform)
{
	localTransform = newTransform;
}

void TerrainNode::AddChild(TerrainNode& child)
{
	child.mParent = this;
	mChildren.emplace_back(child);
}

void TerrainNode::GetNodes(std::vector<TerrainNode*>& outNodes)
{
	if (this->isLeaf)
		outNodes.push_back(this);

	for (auto& child : mChildren)
	{
		child.GetNodes(outNodes);
	}
}

int TerrainNode::GetLod() const
{
	return lod;
}

float TerrainNode::GetGap() const
{
	return gap;
}

glm::vec3 TerrainNode::GetWorldPos() const
{
	return worldPos;
}

glm::vec2 TerrainNode::GetLocation() const
{
	return location;
}

glm::vec2 TerrainNode::Getindex() const
{
	return index;
}
