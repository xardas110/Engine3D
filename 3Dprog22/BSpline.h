#pragma once
#include "Include/glm/glm.hpp"
#include <vector>
#include <array>

#define BSPLINE_CONTEXTS 32

#define BSPLINE_MAX_PTS 300
#define MAX_BSPLINES 1

#define MAX_SPLINE_SEGMENTS 100000

struct BSplineSegment
{
	glm::vec3 a, b;
};

class BSpline
{
	friend class BSplines;
public:
	BSpline();

	void AddPoint(glm::vec3& pt);
	void CheckForReset();
	bool CanReset();
	void GetSegments(std::vector<BSplineSegment>& outSegments);
private:
	int FindKnotInterval(float x);	
	glm::vec3 EvaluateBSplineSimple(float x);
	void ResetSpline();

	int d{ 2 };	
	float drawStep{ 0.8f };
	bool bFirstUpdate{ true };
	std::vector<int> t;
	std::vector<glm::vec3> cp;
	float tMin{ 0.f }; float tMax{ 1.f };
};

struct BSplines
{
	std::array<BSpline, MAX_BSPLINES> splines; int currentIndex = 0; bool bFilled = false;
	bool bSimulate = false;

	void AddPoint(glm::vec3& pt);
	void GetSegments(std::vector<BSplineSegment>& outSegments);
};