#version 460 core

layout(early_fragment_tests) in;

layout(location = 0) out uvec4 b0;

#include "../../Common/GBuffer.frag"
#include "../../Common/ABuffer.frag"

in vec2 texCoords;

uniform sampler2D sunTexture; 
uniform sampler2D sunInnerTexture;

vec4 InnerRGB()
{
	return mix(texture(sunTexture, texCoords), texture(sunInnerTexture, texCoords), 0);
}

void main()
{
	vec4 color = texture(sunTexture, texCoords);
	InsertABufferFragment(ivec2(gl_FragCoord.xy), color, gl_FragCoord.z); discard;
}
