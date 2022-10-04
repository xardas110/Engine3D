#version 460 core

layout(location = 0) out vec4 FragColor;

in vec2 TexCoords_FS;

uniform sampler2D opacitymap;
uniform int numOpacitymaps;

void main()
{
	float alpha = 1.f;
	if (numOpacitymaps != 0)
	{
		alpha = texture(opacitymap, TexCoords_FS).r;
		if (alpha < 0.1f) discard;
	}
	FragColor = vec4(1.f, 1.f, 1.f, alpha);
}
