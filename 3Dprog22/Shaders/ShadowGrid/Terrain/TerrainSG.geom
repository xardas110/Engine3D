#version 460 core

//The shaderparamenter header will replace defines from c++ code. It won't add any new code so the GLSL correction tool won't give syntax errors.

#define MAX_SHADOW_GRIDS 16

layout(triangles, invocations = MAX_SHADOW_GRIDS) in; 
layout(triangle_strip, max_vertices = 3) out;

struct Bounds
{
	vec3 MIN;
	float pad;
	vec3 MAX;
	float pad2;
};

struct ShadowGrid
{
	mat4 projection;
	Bounds bounds;
};

layout (std140, binding=3) uniform ShadowGridUBO
{
	ShadowGrid shadowGrid[MAX_SHADOW_GRIDS];
};

uniform sampler2D heightmap;

void main()
{
	mat4 vp = shadowGrid[gl_InvocationID].projection;
	for (int i = 0; i< 3; i++)
	{
		gl_Position = vp * gl_in[i].gl_Position;		
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	
	EndPrimitive();
}
