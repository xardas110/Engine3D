#include "BSpline.h"
#include "RenderDebugger.h"
#include "Debug.h"

BSpline::BSpline()
{
	cp.reserve(BSPLINE_MAX_PTS);
	t.reserve(BSPLINE_MAX_PTS + d + 1);
}

void BSpline::AddPoint(glm::vec3& pt)
{
	if(bFirstUpdate)
	{ 
		bFirstUpdate = false;

		for (int i = 0; i < d+1; i++)
		{
			cp.emplace_back(pt);
			t.emplace_back(tMin);
		}

		for (int i = 0; i < cp.size(); i++)
		{
			t.emplace_back(tMin + 1.f);
		}
		tMax = tMin + 1.f;
	}
	else
	{
		cp.emplace_back(pt);
		t.emplace_back(t.back());
		for (int i = 0; i < d+1; i++)
		{
			t[t.size() - (1 + i)] = ++tMax;
		}
	}
}

void BSpline::CheckForReset()
{
	if (cp.size() >= BSPLINE_MAX_PTS)
	{
		ResetSpline();
	}
}

bool BSpline::CanReset()
{
	if (cp.size() >= BSPLINE_MAX_PTS) return true;
	return false;
}

void BSpline::GetSegments(std::vector<BSplineSegment>& outSegments)
{
	auto* rd = RenderDebugger::Get();
	for (float i = 0.f; i <= tMax; i += drawStep)
	{
		if (i + drawStep > tMax) break;
		BSplineSegment seg;
		seg.a = EvaluateBSplineSimple(i);
		seg.b = EvaluateBSplineSimple(i + drawStep);
		outSegments.emplace_back(seg);
	}
}

int BSpline::FindKnotInterval(float x)
{
	int i = (int)cp.size() - 1;
	while (x < t[i] && i > d) i--;
	return i;
}


glm::vec3 BSpline::EvaluateBSplineSimple(float x)
{
	int index = FindKnotInterval(x);

	std::vector<glm::vec3> a;
	a.reserve(d + 1);

	for (int i = 0; i < d+1; i++)
	{
		a.emplace_back(glm::vec3( 0.f ));
	}

	for (int j = 0; j <= d; j++)
	{
		a[d - j] = cp[index - j];
	}

	for (int k = d; k > 0; k--)
	{
		int j = index - k;
		for (int i = 0; i < k; i++)
		{
			j++;
			float w = (x - t[j]) / (t[j + k] - t[j]);
			a[i] = a[i] * (1.f - w) + a[i + 1] * w;
		}
	}

	return a[0];
}

void BSpline::ResetSpline()
{
	glm::vec3 lastCP = EvaluateBSplineSimple(tMax - drawStep);
	cp.clear();
	t.clear();
	bFirstUpdate = true;
	tMin = 0.f; tMax = 1.f;
}

void BSplines::AddPoint(glm::vec3& pt)
{
	splines[currentIndex].AddPoint(pt);

	int lastIndex = currentIndex;
	if (splines[currentIndex].CanReset())
	{		
		currentIndex++;

		if (currentIndex >= MAX_BSPLINES)
		{
			currentIndex = 0; bFilled = true;
		}

		if (bFilled)
		{
			splines[currentIndex].ResetSpline();

			glm::vec3 lastPoint = splines[lastIndex].EvaluateBSplineSimple(splines[lastIndex].tMax - splines[lastIndex].drawStep);

			splines[currentIndex].AddPoint(lastPoint);
		}	
		else
		{
			glm::vec3 lastPoint = splines[lastIndex].EvaluateBSplineSimple(splines[lastIndex].tMax - splines[lastIndex].drawStep);

			splines[currentIndex].AddPoint(lastPoint);
		}
	}	
}

void BSplines::GetSegments(std::vector<BSplineSegment>& outSegments)
{
	if (bFilled)
	{ 
		for (size_t i = 0; i < MAX_BSPLINES; i++)
		{
			splines[i].GetSegments(outSegments);
		}
	}
	else
	{
		for (size_t i = 0; i <= currentIndex; i++)
		{
			splines[i].GetSegments(outSegments);
		}
	}
}
