#version 460 core

//The shaderparamenter header will replace defines from c++ code. It won't add any new code so the GLSL correction tool won't give syntax errors.

#define MAX_CASCADES 4

struct Cascade
{
	mat4 cascadeProjection;
	float cascadeOffsetX;
	float cascadeOffsetY;
	float cascadeScaleX;
	float cascadeScaleY;
};

layout (std140, binding=0) uniform CSMMatrices
{
	mat4 toShadowSpaceMat;
	Cascade cascades[MAX_CASCADES];
};

layout(triangles, invocations = MAX_CASCADES) in; //currently only 4 cascades, will make this dynamic later if needed
layout(triangle_strip, max_vertices = 3) out;


in vec2 TexCoords_GS[];
out vec2 TexCoords_FS;

void main()
{
	mat4 vp = cascades[gl_InvocationID].cascadeProjection;
	for (int i = 0; i< 3; i++)
	{
		gl_Position = vp * gl_in[i].gl_Position;		
		gl_Layer = gl_InvocationID;
		TexCoords_FS = TexCoords_GS[i];
		EmitVertex();
	}
	
	EndPrimitive();
}
