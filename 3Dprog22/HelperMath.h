#pragma once
#include "Include/glm/glm.hpp"
#include "Include/glm/gtc/quaternion.hpp"

inline glm::vec3 ConvertMouseScreenSpaceToWorldDir(const float mX, const float mY, const glm::mat4& camInvView, const glm::mat4& camInvProj, const float windowWidth, const float windowHeight)
{
	auto const xHN = ((mX * 2.f) / windowWidth) - 1.f;
	auto const yHN = 1.f - ((mY * 2.f) / windowHeight);
	const auto& inverseView = camInvView;
	const auto& inverseProj = camInvProj;

	glm::vec4 hn(xHN, yHN, -1.f, 1.f); //z = -1.f because openGL uses a RH coordinate system
	glm::vec4 rayClip = inverseProj * hn;
	rayClip = glm::vec4(rayClip.x, rayClip.y, -1.f, 0.f);

	const glm::vec4 rayDir4 = (inverseView * rayClip);
	const glm::vec3 rayDir3(rayDir4.x, rayDir4.y, rayDir4.z);
	return glm::normalize(rayDir3);
}

inline glm::vec2 ConvertToScreenSpace(const glm::vec3& ws, const glm::mat4& view, const glm::mat4& proj)
{
	auto coords = glm::vec4(ws.x, ws.y, ws.z, 1.f);

	coords = view * coords;
	coords = proj * coords;

	float x = (coords.x / coords.w + 1.f) * 0.5f;
	float y = 1.f - ((coords.y / coords.w + 1.f) * 0.5f);

	return { x, y };
}

inline bool Cmp(const float a, const float b, const float epsilon = 0.0005f)
{
	return (fabs(a - b) < epsilon);
}

inline void	CalculateYawPitchFromVector(const glm::vec3& vec, float& yaw, float& pitch)
{
	pitch = acos(vec.z);
	
	const float cosinedYaw = vec.x / (cos(pitch) + FLT_EPSILON);
	yaw = acos(cosinedYaw);
}
template<class T>
inline T Min(T a, T b)
{
	if (a < b)
		return a;
	
	return b;
}
inline void DecomposeMatrix(const glm::mat4& m, glm::vec3& position, glm::quat& rotation, glm::vec3& scale)
{
	position = m[3];
	
	for (int i = 0; i < 3; i++)
		scale[i] = glm::length(glm::vec3(m[i]));
	
	const glm::mat3 rotMtx(
		glm::vec3(m[0]) / scale[0],
		glm::vec3(m[1]) / scale[1],
		glm::vec3(m[2]) / scale[2]);
	
	rotation = glm::quat_cast(rotMtx);
}

/*Not cross safe, needs normalized data*/
inline glm::mat4 GetLookAt(const glm::vec3& dir, const glm::vec3& worldUp)
{
	glm::vec3 right = glm::cross(worldUp, dir);
	glm::vec3 newUp = glm::cross(dir, right);
	return
	{
		{right, 0.f},
		{worldUp, 0.f},
		{dir, 0.f},
		{0.f, 0.f, 0.f, 1.f},
	};
}

template<class T>
inline void SafeNormal(T& n)
{
	T temp = n;
	temp = glm::normalize(temp);

	if (glm::any(glm::isnan(temp)))
	{
		return;
	};

	n = glm::normalize(n);
}

template<class T>
inline bool IsNan(const T& t)
{
	return glm::any(glm::isnan(t));
}