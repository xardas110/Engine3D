#version 460 core
layout (location = 0) in vec3 pos0;   
layout (location = 1) in vec2 texCoords0;

struct Character
{
	vec4 posOffset; //pos and scale
	vec4 texcoordOffset; //pos and scale	
};

layout (std430, binding=7) buffer instancedCharactersSSBO
{
	Character characters[];
};

uniform mat4 model;
uniform mat4 VP;

out vec2 TexCoords;

void main()
{
	Character c = characters[gl_InstanceID];

	c.posOffset.xy = c.posOffset.xy * 2.f - 1.f;

    gl_Position = VP * model * vec4(pos0.xy * c.posOffset.zw + c.posOffset.xy, 0.0, 1.0);
    TexCoords = texCoords0 * c.texcoordOffset.zw + c.texcoordOffset.xy;
}  