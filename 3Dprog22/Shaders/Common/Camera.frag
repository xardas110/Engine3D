
layout(std140, binding=2) uniform CameraUBO
{
	mat4 view;
	mat4 proj;
	mat4 VP;
	vec3 viewPos;
	float cameraZFar;
	int time;
};