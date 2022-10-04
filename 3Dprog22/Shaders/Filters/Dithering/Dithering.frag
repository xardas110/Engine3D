#version 460 core
	
in vec2 TexCoords;

uniform sampler2D noiseTexture;
uniform sampler2D source;
uniform layout(binding=3, rgba16f) writeonly image2D volumetricFiltered;

void main()
{
	vec3 sourcePixel = texture2D(source, TexCoords).rgb;
	float grayscale = length(sourcePixel*vec3(0.2126,0.7152,0.0722));
	
	vec3 ditherPixel = texture2D(noiseTexture, vec2(mod(gl_FragCoord.xy/vec2(8.0,8.0),1.0))).xyz;
	float ditherGrayscale = (ditherPixel.x + ditherPixel.y + ditherPixel.z) / 3.0;
	ditherGrayscale -= 0.5;
	
	float ditheredResult = grayscale + ditherGrayscale;
	
	float bit = ditheredResult >= 0.5 ? 1.0 : 0.0;

	vec4 color = texture(source, TexCoords) * bit;
	imageStore(volumetricFiltered, ivec2(gl_FragCoord.xy), vec4(color.xyz, 1.f));
	discard;
}
