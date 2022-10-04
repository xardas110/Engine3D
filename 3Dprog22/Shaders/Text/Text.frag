#version 460 core

layout(early_fragment_tests) in;

out uvec4 b0;

uniform sampler2D atlas;
uniform vec3 textColor;

#include "../common/gbuffer.frag"
#include "../common/abuffer.frag"
#include "../common/ShaderModelDefines.frag"

in vec2 TexCoords;

void main()
{    
    vec4 c = texture(atlas, TexCoords);

    vec4 color = vec4(textColor, c.a);

    InsertABufferFragment(ivec2(gl_FragCoord.xy), color, gl_FragCoord.z);
    discard;
}  