#version 460 core
out vec4 blurColor;

uniform sampler2D source;
uniform bool bHorizontal;
uniform int n;

uniform float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

in vec2 TexCoords;

//source: https://learnopengl.com/Advanced-Lighting/Bloom
//few changes made
void main(void)
{
	const vec2 sourceDim = vec2(textureSize(source, 0));

	vec2 texelSize = 1.f / sourceDim;

	vec4 result = texture(source, TexCoords) * weights[0];
	
	if(bHorizontal)
    {
        for(int i = 1; i < n; ++i)
        {
            result += texture(source, TexCoords + vec2(texelSize.x * i, 0.0)) * weights[i];
            result += texture(source, TexCoords - vec2(texelSize.x * i, 0.0)) * weights[i];
        }
    }
    else
    {
        for(int i = 1; i < n; ++i)
        {
            result += texture(source, TexCoords + vec2(0.0, texelSize.y * i)) * weights[i];
            result += texture(source, TexCoords - vec2(0.0, texelSize.y * i)) * weights[i];
        }
    }

	blurColor = result;
}