#version 460 core

out vec4 fragColor;
in vec2 TexCoords_FS;

uniform sampler2D opacitymap;
uniform int numOpacitymaps;

void main(){

	float alpha = 1.f;

	if (numOpacitymaps != 0)
	{
		alpha = texture(opacitymap, TexCoords_FS).a;
		if (alpha < 0.01f) discard;
	}

	fragColor = vec4(1.f, 1.f, 1.f, alpha);
}
