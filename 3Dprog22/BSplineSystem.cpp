#include "BSplineSystem.h"
#include "World.h"
#include "BSpline.h"
#include "Components.h"
#include "RenderDebugger.h"
#include "RenderEngine.h"

BSplineSystem::BSplineSystem()
{
	thPool = new ThreadPoolx([this](int thIndex) {this->THUpdate(thIndex); }, BSPLINE_CONTEXTS);
}

BSplineSystem::~BSplineSystem()
{
	delete thPool;
	thPool = nullptr;

	glDeleteBuffers(1, &segmentVBO);
	glDeleteVertexArrays(1, &segmentVAO);
}

void BSplineSystem::InitShader()
{
	auto* re = RenderEngine::Get();
	re->GetShader(ShaderManager::DefaultDebug, shader);
	re->BindShader(shader);
}

void BSplineSystem::Init(World* world, entt::registry& registry)
{
	this->world = world;
	this->registry = &registry;
	GenerateLineSegment();
	InitShader();
}

void BSplineSystem::GenerateLineSegment()
{
	initializeOpenGLFunctions();

	unsigned int VAO, VBO;

	initializeOpenGLFunctions();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_SPLINE_SEGMENTS * sizeof(BSplineSegment), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)NULL);

	segmentVBO = VBO;
	segmentVAO = VAO;
}

void BSplineSystem::PreUpdate(float deltatime)
{
	currentTimer += deltatime;	
	thPool->StartWork();
}

void BSplineSystem::PostUpdate(float deltatime)
{
	thPool->WaitWork();
}

void BSplineSystem::RenderSegments(std::vector<BSplineSegment>& inSegments)
{
	auto* re = RenderEngine::Get();
	re->BindShader(shader);
	re->EnableDepthTest();
	re->SetDepthMask(true);
	glBindVertexArray(segmentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, segmentVBO);

	glBufferSubData(GL_ARRAY_BUFFER, 0, inSegments.size() * sizeof(BSplineSegment), inSegments.data());
	glDrawArrays(GL_LINES, 0, inSegments.size() * 2);
}

void BSplineSystem::Render()
{
	auto* rd = RenderDebugger::Get();
	if (currentTimer >= waitTime)
	{
		currentTimer = 0.f;

		for (int i = 0; i < BSPLINE_CONTEXTS; i++)
		{
			bDrawn[i] = false;
		}
	}

	for (int i = 0; i < BSPLINE_CONTEXTS; i++) RenderSegments(lines[i]);	
}

void BSplineSystem::THUpdate(int thIndex)
{
	if (currentTimer >= waitTime && !bDrawn[thIndex])
	{
		lines[thIndex].clear();
		auto view = registry->view<BSplines>();
		for (int i = thIndex; i < view.size(); i += BSPLINE_CONTEXTS)
		{
			auto& bSpline = view.get<BSplines>(view[i]);
			auto& trans = registry->get<TransformComponent>(view[i]);
			auto pos = trans.GetPosition();
			if (!bSpline.bSimulate) continue;
			bSpline.AddPoint(pos);
			bSpline.GetSegments(lines[thIndex]);
		}	
		bDrawn[thIndex] = true;
	}
}
