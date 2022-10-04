#version 460 core
out float blurColor;

uniform sampler2D source;

in vec2 TexCoords;

const int uSize = 2;
const int vSize = 2;

void main(void)
{
	const vec2 sourceDim = vec2(textureSize(source, 0));

	vec2 texelSize = 1.f / sourceDim;

	float result = 0.f;
	
	for (int u = -uSize; u < uSize; u++)
	{
		for(int v= -vSize; v < vSize; v++)
		{
			vec2 offset = vec2(float(u), float(v)) * texelSize;
			result += texture(source, TexCoords + offset).r;
		}
	}

	blurColor = result / (4.f * 4.f);
}