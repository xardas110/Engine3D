#version 460

layout(quads, fractional_odd_spacing, cw) in;

in vec2 texCoords_TE[];

uniform float scaleY;
uniform sampler2D heightmap;

void main(){

    const float u = gl_TessCoord.x;
    const float v = gl_TessCoord.y;
	const float iu = 1 - u;
	const float iv = 1 -v;

	vec4 position =
	(gl_in[12].gl_Position * iu * iv +
	gl_in[0].gl_Position * u * iv +
	gl_in[3].gl_Position * u * v +
	gl_in[15].gl_Position * iu * v);
	
	vec2 mapCoord =
	(texCoords_TE[12] * iu * iv +
	texCoords_TE[0] * u * iv +
	texCoords_TE[3] * u * v +
	texCoords_TE[15]* iu * v);
	
	float height = texture(heightmap, mapCoord).r * scaleY;

	position.y = height;

	gl_Position = position;
}