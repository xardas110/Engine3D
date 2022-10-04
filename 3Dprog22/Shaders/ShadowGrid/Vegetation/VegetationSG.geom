#version 460 core

#define MAX_SHADOW_GRIDS 4

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

in vec2 TexCoords_GS[];
out vec2 TexCoords_FS;

void main()
{
	mat4 vp = shadowGrid[gl_InvocationID].projection;
	for (int i = 0; i< 3; i++)
	{
		gl_Position = vp * gl_in[i].gl_Position;		
		gl_Layer = gl_InvocationID;
		TexCoords_FS = TexCoords_GS[i];
		EmitVertex();
	}
	
	EndPrimitive();
}
