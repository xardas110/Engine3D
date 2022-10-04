#include "BezierFunction.h"
#include "PointGenerator.h"

glm::vec2 BezierFunction::Func(float t)
{
	return PointGenerator::GenerateBezierPoint(pts, t);
}

void BezierFunction::SetPoints(const std::vector<glm::vec2>& pts)
{
	this->pts = pts;
	GeneratePoints();
}

const std::vector<glm::vec2>& BezierFunction::GetPoints() const
{
	return pts;
}

void BezierFunction::SetTransform(const glm::mat4& newTrans)
{
	transform = newTrans;
}

const glm::mat4& BezierFunction::GetTransform() const
{
	return transform;
}

const std::vector<glm::vec3> BezierFunction::GetTransformedPoints() const
{
	std::vector<glm::vec3> temp;

	for (size_t i = 0; i < pts.size(); i++)
	{
		auto& pt = pts[i];

		glm::vec4 pt4 = glm::vec4(pt.x, 0.f, pt.y, 1.f);

		glm::vec3 finalPos = transform * pt4;
		temp.emplace_back(finalPos);
	}

	return temp;
}

const std::vector<glm::vec3> BezierFunction::GetGeneratedPoints() const
{
	std::vector<glm::vec3> temp;

	for (size_t i = 0; i < generatedPts.size(); i++)
	{
		auto& pt = generatedPts[i];

		glm::vec4 pt4 = glm::vec4(pt.x, 0.f, pt.y, 1.f);

		glm::vec3 finalPos = transform * pt4;
		temp.emplace_back(finalPos);
	}

	return temp;
}

void BezierFunction::GeneratePoints()
{
	generatedPts =  PointGenerator::GenerateBezierPoints(pts, 0.f, 1.f, 0.05f);
}
