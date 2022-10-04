#version 460 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D posBuffer;
uniform sampler2D colorBuffer;
uniform vec3 fogColor;
uniform float zFar;
uniform float sightRange;

layout(std140, binding=2) uniform CameraUBO
{
	mat4 view;
	mat4 proj;
	mat4 VP;
	vec3 viewPos;
	float padding;
};

in vec2 TexCoords;

float GetFogFactor(float dist)
{
	return -0.0002f/sightRange*(dist-zFar/10.f*sightRange) +1;
}

void main()
{
	vec3 fragPos = texture(posBuffer, TexCoords).rgb;
	vec3 color = texture(colorBuffer, TexCoords).rgb;
	float dist = length(viewPos - fragPos);
	float fogFactor = GetFogFactor(dist); 
	
	vec3 result = mix(fogColor, color, clamp(fogFactor, 0.f, 1.f));

	FragColor = vec4(result, 1.f);
}
