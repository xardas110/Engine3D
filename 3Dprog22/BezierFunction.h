#pragma once
#include <vector>
#include "Include/glm/glm.hpp"

//oppgave 7
class BezierFunction
{
public:
	using Points = std::vector<glm::vec2>;

	glm::vec2 Func(float t);

	void SetPoints(const std::vector<glm::vec2>& pts);
	const Points& GetPoints() const;

	void SetTransform(const glm::mat4& newTrans);
	const glm::mat4& GetTransform() const;

	const std::vector<glm::vec3> GetTransformedPoints() const;

	const std::vector<glm::vec3> GetGeneratedPoints() const;
private:
	void GeneratePoints();

	Points pts;
	Points generatedPts;

	glm::mat4 transform{1.f};
};

