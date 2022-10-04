#version 460

#define MAX_CASCADES 4

#include "../../../common/Camera.frag"

layout(triangles, invocations = MAX_CASCADES) in;
layout(triangle_strip, max_vertices = 3) out;

struct Material
{
	sampler2D normalmap;
	sampler2D heightmap;
	
	float heightScaling;
	float horizontalScaling;
};

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

uniform sampler2D normalmap;
uniform int tessellationRange;
uniform sampler2D splatmap;
uniform Material materials[3];

vec3 tangent;
vec3 displacement[3];

void CalcuateTangent();
void CalculateDisplacement();

in vec2 mapCoord_GS[];

void main() 
{
	tangent = vec3(0.f, 0.f, 0.f);
	CalculateDisplacement();
	
	mat4 cascadeVP = cascades[gl_InvocationID].cascadeProjection;

	for (int i = 0; i < gl_in.length(); ++i)
	{
		vec4 worldPos = gl_in[i].gl_Position + vec4(displacement[i],0);
		gl_Position = cascadeVP * worldPos;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	
	EndPrimitive();
}

void CalcuateTangent()
{
	vec3 v0 = gl_in[0].gl_Position.xyz;
	vec3 v1 = gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[2].gl_Position.xyz;

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
	
	vec2 uv0 = mapCoord_GS[0];
	vec2 uv1 = mapCoord_GS[1];
	vec2 uv2 = mapCoord_GS[2];

    vec2 deltaUV1 = uv1 - uv0;
	vec2 deltaUV2 = uv2 - uv0;
	
	float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	
	tangent = normalize((e1 * deltaUV2.y - e2 * deltaUV1.y)*r);
}

void CalculateDisplacement()
{
	for (int i = 0; i < 3; ++i)
	{
		displacement[i] = vec3(0,0,0);
	}

	float dist = (distance(gl_in[0].gl_Position.xyz, viewPos)
				 + distance(gl_in[1].gl_Position.xyz, viewPos) 
				 + distance(gl_in[2].gl_Position.xyz, viewPos)) / 3.f;
	
	if (dist < tessellationRange)
	{
	
		CalcuateTangent();
		
		for(int i=0; i<gl_in.length(); i++)
		{ 
			
			displacement[i] = vec3(0,1,0);
			
			float height = gl_in[i].gl_Position.y;
			
			vec3 normal = normalize(texture(normalmap, mapCoord_GS[i]).rbg);
			
			vec4 blendValues = texture(splatmap, mapCoord_GS[i]).rgba;
	
			float[4] blendValueArray = float[](blendValues.r,blendValues.g,blendValues.b,blendValues.a);
			
			float scale = 0;
			for (int j=0; j<3; j++)
			{
				scale += texture(materials[j].heightmap, mapCoord_GS[i]
							* materials[j].horizontalScaling).r 
							* materials[j].heightScaling 
							* blendValueArray[j];
			}
						
			float attenuation = clamp(- distance(gl_in[i].gl_Position.xyz, viewPos)/(tessellationRange-50) + 1,0.0,1.0);
			scale *= attenuation;

			displacement[i] *= scale;
		}	
	}
}