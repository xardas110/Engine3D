#pragma once
#include <vector>
#include "Mesh.h"
#include "components.h"
#include "shader.h"
#include "SSBO.h"
#include <QOpenGLFunctions_4_5_Core>
#include <mutex>
#include <array>
#include <atomic>
#include <BSplineSystem.h>

#define MAX_DEBUG_ELEMENTS 10000000

struct GpuData
{
	glm::mat4 model{ 1.f };
	glm::vec4 color{1.f, 0.f, 0.f, 1.f};
};

struct DebugInfo
{
	float duration{ 0.f };
	float width{ 1.f };
	bool depthPass{ true };
	glm::vec3 color{ 1.f, 0.f, 0.f };
};
struct DebugSegment : public DebugInfo
{
	glm::vec3 a{}, b{};
};
struct DebugPoint : public DebugInfo
{
	glm::vec3 a{};
};
struct DebugBox : public DebugInfo
{
	glm::vec3 c;
	glm::vec3 e;
	glm::quat rot;
};
struct DebugSphere : public DebugInfo
{
	glm::vec3 c;
	float r;
};

class RenderDebugger : protected QOpenGLFunctions_4_5_Core
{
	friend class World;
	friend class DeferredRenderer;

	RenderDebugger();
public:
	
	static RenderDebugger* Get();

	void AddDebugSegment(float timer, const glm::vec3& a, const glm::vec3& b, const glm::vec3& color = { 1.f, 0.f, 0.f }, float size = 1.f, bool bDepthTest = true);
	void AddDebugPoint(float timer, const glm::vec3& a, const glm::vec3& color = { 1.f, 0.f, 0.f }, float size = 5.f, bool bDepthTest = true);
	void AddDebugBox(float timer, const glm::vec3& c, const glm::vec3& e, const glm::quat& rot,  const glm::vec3 color = { 1.f, 0.f, 0.f }, float size = 5.f, bool bDepthTest = true);
	void AddDebugBoundingBox(float timer, const glm::vec3& min, const glm::vec3& max, const glm::vec3 color = { 1.f, 0.f, 0.f }, float size = 5.f, bool bDepthTest = true);
	void AddDebugSphere(float timer, const glm::vec3& c, float r, const glm::vec3& color = { 1.f, 0.f, 0.f }, float size = 5.f, bool bDepthTest = true);

private:
	void InitShader();
	void InitSSBO();

	void Render(float deltatime);

	void DrawDebugSegments(float deltatime);
	void DrawDebugPoints(float deltaTime);
	void DrawDebugSphere(float deltatime);
	void DrawDebugBoxes(float deltatime);
	
	void GenerateDebugSphere(int n);
	void GenerateDebugBox();
	void GenerateDebugPoint();
	void GenerateDebugSegment();

	void ClearContainers();

	Mesh box;
	Mesh point;
	Mesh sphere;
	Mesh segment;

	Shader shader;

	SSBO ssbo;

	unsigned colorUniform;

	std::mutex mMutex;

	std::vector<DebugPoint> debugPoints;
	std::vector<DebugSegment> debugSegments;	
	std::vector<DebugSphere> debugSpheres;
	std::vector<DebugBox> debugBoxes;
};


