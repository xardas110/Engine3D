#version 460

layout(vertices = 16) out;

in vec2 texCoords_TC[];
out vec2 texCoords_TE[];

void main()
{
	//shadowgrid will not use any lod levels.
	if (gl_InvocationID == 0)
	{	
		gl_TessLevelOuter[0] = 16;
		gl_TessLevelOuter[1] = 16;
		gl_TessLevelOuter[2] = 16;
		gl_TessLevelOuter[3] = 16;
		
		gl_TessLevelInner[0] = 16;
		gl_TessLevelInner[1] = 16;
	}
	
	texCoords_TE[gl_InvocationID] = texCoords_TC[gl_InvocationID];
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
