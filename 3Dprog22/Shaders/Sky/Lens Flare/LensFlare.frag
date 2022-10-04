#version 460 core
out vec4 FragColor;
  
in vec2 texCoords;

uniform sampler2D lensTexture; 
uniform float brightness;


void main()
{
	vec4 texColor = texture(lensTexture, texCoords);
	texColor.a *= brightness;

	FragColor = texColor;
}
