#include "Camera.h"
#include <iostream>
#include "HelperMath.h"
#include "Include/glm/gtc/matrix_transform.hpp"

//#include "Debug.h"

void Camera::UpdateViewMatrix()
{
	right = glm::normalize(glm::cross(dir, WORLD_UP));
	up = glm::normalize(glm::cross(right, dir));
	view = glm::lookAt(pos, pos + dir, WORLD_UP);
}

void Camera::UpdateProjectionMatrix()
{
	projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

FrustumCorners::FrustumArray Camera::GetFrustumCorners() const
{
	FrustumCorners::FrustumArray corners{};

	auto camProj = glm::inverse(projection);
	
	const glm::vec3 cr = right;
	const glm::vec3 cu = up;
	const glm::vec3 cd = dir; // TODO: something strange cam view 2 is expected here
	const glm::vec3 cp = pos;

	/*Perspective width scale*/
	const float cws = camProj[0][0];
	/*Perspective height scale*/
	const float chs = camProj[1][1];
	
	const float cn = near;
	const float cf = far;

	corners[FrustumCorners::LeftTopNear]	= cp - cr * cn * cws + cu * cn * chs + cd * cn;
	corners[FrustumCorners::LeftBotNear]	= cp - cr * cn * cws - cu * cn * chs + cd * cn;
	corners[FrustumCorners::RightBotNear]	= cp + cr * cn * cws - cu * cn * chs + cd * cn;
	corners[FrustumCorners::RightTopNear]	= cp + cr * cn * cws + cu * cn * chs + cd * cn;

	corners[FrustumCorners::LeftTopFar]		= cp - cr * cf * cws + cu * cf * chs + cd * cf;
	corners[FrustumCorners::LeftBotFar]		= cp - cr * cf * cws - cu * cf * chs + cd * cf;
	corners[FrustumCorners::RightBotFar]	= cp + cr * cf * cws - cu * cf * chs + cd * cf;
	corners[FrustumCorners::RightTopFar]	= cp + cr * cf * cws + cu * cf * chs + cd * cf;

	return corners;
}

Camera::Camera()
	:velocity({0.f, 0.f, 0.f}) //:fov(45.f), aspect(0.f), near(0.1f), far(100.f)
{
	pitch = 0.f;
	yaw = -90.f;
	
	fov = 45.f;
	aspect = (float)1920 / (float)1080;
	near = 0.1f;
	far = 1000.f;
	mouseSensitivity = 0.1f;
	
	const glm::vec3 camPos = { 0.f, 0.f, 10.f };
	const glm::vec3 camDir = { 0.f, 0.f, -1.f };
	pos = camPos;
	dir = camDir;
	view = glm::lookAt(camPos, camPos + camDir, WORLD_UP);
	projection = glm::perspective(fov, aspect, near, far);
}

void Camera::SetCameraDirection(const float yaw, const float pitch)
{
	this->yaw = yaw;
	this->pitch = pitch;

	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir = glm::normalize(dir);

	UpdateViewMatrix();
}

void Camera::SetCameraPosition(const glm::vec3& pos)
{
	this->pos = pos;
	UpdateViewMatrix();
}

void Camera::ProcessMouseMovement(float x, float y)
{
	yaw += (x*mouseSensitivity);
    pitch -= (y*mouseSensitivity);
	
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	
	SetCameraDirection(yaw, pitch);
}

void Camera::SetCameraDirection(const glm::vec3& dir)
{
	if (this->dir == dir)
		return;
	
	this->dir = glm::normalize(dir);

	CalculateYawPitchFromVector(this->dir, this->yaw, this->pitch);
	
	UpdateViewMatrix();
}

void Camera::SetProjection(float fov, float aspect, float near, float far)
{
	SetFov(fov);
	SetAspect(aspect);
	SetNear(near);
	SetFar(far);
}

void Camera::SetFov(const float fov)
{
	if (Cmp(this->fov, fov, std::numeric_limits<float>::epsilon()))
		return;

	this->fov = fov;

	UpdateProjectionMatrix();
}

void Camera::SetAspect(const float aspect)
{
	if (Cmp(this->aspect, aspect, std::numeric_limits<float>::epsilon()))
		return;
	
	this->aspect = aspect;
	
	UpdateProjectionMatrix();
}

void Camera::SetNear(const float near)
{
	if (Cmp(this->near, near, std::numeric_limits<float>::epsilon()))
		return;

	this->near = near;

	UpdateProjectionMatrix();
}

void Camera::SetFar(const float far)
{
	if (Cmp(this->far, far, std::numeric_limits<float>::epsilon()))
		return;

	this->far = far;

	UpdateProjectionMatrix();
}

void Camera::AddTranslate(const glm::vec3& trans)
{
	pos += trans;
	UpdateViewMatrix();
}

void Camera::MoveForward(float scale, float deltaTime)
{
	if (abs(scale) > 1) return; //validation
	
	SetCameraPosition(pos + (scale * dir * deltaTime * cameraSpeed));
}

void Camera::MoveRight(float scale, float deltaTime)
{
	if (abs(scale) > 1) return;

	SetCameraPosition(pos + (scale * right * deltaTime * cameraSpeed));
}

void Camera::MoveUp(float scale, const float deltatime)
{
	if (abs(scale) > 1) return;

	SetCameraPosition(pos + (scale * up * deltatime * cameraSpeed));
}

void Camera::SetCameraSpeed(float speed)
{
	cameraSpeed = speed;
}

const glm::vec3 Camera::GetPosition() const
{
	return pos;
}

void Camera::SetVelocity(const glm::vec3& velocity)
{
	this->velocity = velocity;
}

void Camera::SetVelocityX(const float x)
{
	SetVelocity({ x, velocity.y, velocity.z });
}

void Camera::SetVelocityY(const float y)
{
	SetVelocity({ velocity.x, y, velocity.z });
}

void Camera::SetVelocityZ(const float z)
{
	SetVelocity({ velocity.x, velocity.y, z });
}

void Camera::UpdateCamera(const float deltaTime)
{
	AddTranslate(velocity * deltaTime);
}


const glm::mat4 Camera::GetViewMatrix() const
{
	return view;
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
	return projection;
}

float Camera::GetNear() const
{
	return near;
}

float Camera::GetFar() const
{
	return far;
}

float Camera::GetFov() const
{
	return fov;
}

glm::vec3 Camera::GetDirection() const
{
	return dir;
}
