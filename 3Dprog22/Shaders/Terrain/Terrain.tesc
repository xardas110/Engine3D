#version 460

//https://www.youtube.com/watch?v=z03vg2QTA8k&t=2053s&ab_channel=OREONENGINE

layout(vertices = 16) out;

#include "../common/camera.frag"

in vec2 mapCoord_TC[];
out vec2 mapCoord_TE[];

uniform int tessellationFactor;
uniform float tessellationSlope;
uniform float tessellationShift;
uniform bool bEnableTess;

float LodFactor(float dist); 

void main(){
	
	if (gl_InvocationID == 0)
	{
		if (bEnableTess)
		{
			const vec3 abMid = vec3(gl_in[0].gl_Position + gl_in[3].gl_Position)/2.0;	
			const vec3 bcMid = vec3(gl_in[3].gl_Position + gl_in[15].gl_Position)/2.0;
			const vec3 cdMid = vec3(gl_in[15].gl_Position + gl_in[12].gl_Position)/2.0;
			const vec3 daMid = vec3(gl_in[12].gl_Position + gl_in[0].gl_Position)/2.0;		
		
			float distAB = distance(abMid, viewPos);
			float distBC = distance(bcMid, viewPos);
			float disdCD = distance(cdMid, viewPos);
			float distDA = distance(daMid, viewPos);
		
			gl_TessLevelOuter[2] = mix(1, gl_MaxTessGenLevel, LodFactor(distAB));
			gl_TessLevelOuter[3] = mix(1, gl_MaxTessGenLevel, LodFactor(distBC));
			gl_TessLevelOuter[0] = mix(1, gl_MaxTessGenLevel, LodFactor(disdCD));
			gl_TessLevelOuter[1] = mix(1, gl_MaxTessGenLevel, LodFactor(distDA));
		
			gl_TessLevelInner[0] = (gl_TessLevelOuter[3] + gl_TessLevelOuter[1])/4;
			gl_TessLevelInner[1] = (gl_TessLevelOuter[2] + gl_TessLevelOuter[0])/4;
		}
		else
		{
			gl_TessLevelOuter[2] = 1;
			gl_TessLevelOuter[3] = 1;
			gl_TessLevelOuter[0] = 1;
			gl_TessLevelOuter[1] = 1;
		
			gl_TessLevelInner[0] = 1;
			gl_TessLevelInner[1] = 1;
		}
	}
	
	mapCoord_TE[gl_InvocationID] = mapCoord_TC[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}

float LodFactor(float dist)
{
	return max(0.0, tessellationFactor/pow(dist, tessellationSlope) + tessellationShift);
}