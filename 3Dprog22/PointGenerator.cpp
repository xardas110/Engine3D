
#include "PointGenerator.h"


#include <iostream>
#include <ostream>

std::vector<glm::vec2> PointGenerator::Generate(std::function<float(float)> func, float start, float end, float step)
{
	std::vector<glm::vec2> result;
	for (float i = start; i< end; i += step)
		result.emplace_back(glm::vec2{ i , func(i) });
	
	return result;
}

glm::vec2 PointGenerator::GenerateBezierPoint(std::vector<glm::vec2> pts, float t)
{
	if (pts.size() == 2)
	{
		return (1.f - t)* pts[0] + t*pts[1];
	}
	std::vector<glm::vec2> newPts;
	for (auto i = 0; i< pts.size()-1; i++)
	{	
		const auto ci = i;
		const auto ni = i + 1;	
		glm::vec2 newPos = (1.f - t) * pts[ci] + t * pts[ni];		
		newPts.push_back(newPos);			
	}
	return PointGenerator::GenerateBezierPoint(newPts, t);
	
	assert(false && "End of recursion");
}

std::vector<glm::vec2> PointGenerator::GenerateBezierPoints(std::vector<glm::vec2> pts, float st, float et, float step)
{
	std::vector<glm::vec2> result;
	for (float i = 0.f; i <= et+FLT_EPSILON; i += step)
	{
		result.emplace_back(GenerateBezierPoint(pts, i));
	}
	return result;
}
