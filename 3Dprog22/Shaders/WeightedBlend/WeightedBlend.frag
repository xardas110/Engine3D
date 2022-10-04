
#define AB_BUFFER_WIDTH 3840
#define AB_BUFFER_HEIGHT 2160

#define BUFFER_SIZE AB_BUFFER_WIDTH * AB_BUFFER_HEIGHT

struct Fragment //Max 16 bytes
{
	vec4 accum; 
	vec4 reveal;
};

layout(std430, binding = 5) buffer AlphaBuffer
{
	Fragment alphaNode[];
};

bool CMP(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * 0.0005f;
}

float GetWeight(float z, in vec4 color)
{

	float part1 = min(1.0, max(max(color.r, color.g), color.b) * color.a);
	float part2 = clamp(0.03 / (1e-5 + pow(z / 200, 10.0)), 1e-2, 3e3);

	return max(part1, color.a);
      
}

Fragment GetAlphaFragment(in ivec2 coords)
{
	int index = coords.y * AB_BUFFER_WIDTH + coords.x;
	return alphaNode[index];
}

void ClearAlphaFragment(in ivec2 coords)
{
	int index = coords.y * AB_BUFFER_WIDTH + coords.x;
	alphaNode[index].accum = vec4(0.f, 0.f, 0.f, 0.f);
	alphaNode[index].reveal = vec4(1.f, 0.f, 0.f, 0.f);
}

void InsertAlphaFragment(in ivec2 coords, float currentDepth, in vec4 color)
{
	int index = coords.y * AB_BUFFER_WIDTH + coords.x;
	Fragment frag = alphaNode[index];

	
	float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * 
                        pow(1.0 - currentDepth * 0.9, 3.0), 1e-2, 3e3);

	Fragment result;
	result.accum = vec4(color.rgb * color.a, color.a) * weight;
	result.accum = result.accum + frag.accum;

	/*
	float srcAlpha = 1.f - frag.reveal.x;
	float dstAlpha = 1.f - srcAlpha;
	
	vec3 result = avgColor.rgb * srcAlpha + color.rgb * dstAlpha;

	float alpha = frag.reveal.x * srcAlpha + color.a * dstAlpha;
	*/

	float dstBlend = 1.f - color.a;
	result.reveal = vec4(frag.reveal * dstBlend);
	result.reveal.z = currentDepth;

	alphaNode[index] = result;
}

void AddAlphaColor(in ivec2 coords, float depth, inout vec4 color)
{
	int index = coords.y * AB_BUFFER_WIDTH + coords.x;

	Fragment frag = alphaNode[index];

	if (CMP(frag.reveal.x, 1.0f))
        return;

	vec4 avgColor = vec4(frag.accum.rgb / max(frag.accum.a, 1e-5), frag.reveal.x);
	
	//float srcAlpha = 1.f - frag.reveal.x;
	//float dstAlpha = srcAlpha;
	
	//vec3 result = avgColor.rgb * srcAlpha + color.rgb * dstAlpha;

	//float alpha = frag.reveal.x * srcAlpha + color.a * dstAlpha;
	
	vec4 result = mix(avgColor, color, frag.reveal.x);
	color = vec4(result.rgb, 1.f);
}