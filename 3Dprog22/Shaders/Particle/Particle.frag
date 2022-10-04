#version 460 core
out uvec4 b0;

layout(early_fragment_tests) in;

in vec2 TexCoords_FS;
in vec4 ParticleColor_FS;

#include "../common/abuffer.frag"

uniform sampler2D sprite;

void main()
{
	vec4 color = texture(sprite, TexCoords_FS);
	InsertABufferFragment(ivec2(gl_FragCoord.xy), color, gl_FragCoord.z); discard;	
}