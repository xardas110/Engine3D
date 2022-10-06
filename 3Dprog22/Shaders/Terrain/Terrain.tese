#version 460

//https://www.youtube.com/watch?v=z03vg2QTA8k&t=2053s&ab_channel=OREONENGINE

layout(quads, fractional_odd_spacing, cw) in;

in vec2 mapCoord_TE[];

out vec2 mapCoord_GS;

uniform float scaleY;

uniform sampler2D heightmap;


void main(){

    const float u = gl_TessCoord.x;
    const float v = gl_TessCoord.y;
	const float iu = 1 - u;
	const float iv = 1 - v;

	vec4 position =
	(gl_in[12].gl_Position * iu * iv +
	gl_in[0].gl_Position * u * iv +
	gl_in[3].gl_Position * u * v +
	gl_in[15].gl_Position * iu * v);
	
	vec2 mapCoord =
	(mapCoord_TE[12] * iu * iv +
	mapCoord_TE[0] * u * iv +
	mapCoord_TE[3] * u * v +
	mapCoord_TE[15]* iu * v);
	
	float height = texture(heightmap, mapCoord).r * scaleY;

	position.y = height;

	mapCoord_GS = mapCoord;

	gl_Position = position;
}