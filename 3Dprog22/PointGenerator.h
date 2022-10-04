#pragma once
#include <array>
#include <functional>
#include <vector>
#include "Include/glm/vec2.hpp"

struct PointGenerator
{
	static std::vector<glm::vec2> Generate(std::function<float(float)> func, float start, float end, float step);
	
	static std::vector<glm::vec2> GenerateBezierPoints(std::vector<glm::vec2> pts, float st, float et, float step);

	/*4 points bezier only, for fast meth*/
	static glm::vec2 GenerateBezierPoint(std::vector<glm::vec2> pts, float t);

};

