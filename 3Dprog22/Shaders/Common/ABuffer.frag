
#define MAX_FRAGMENTS 50
#define MAX_NODES 1920*1080 * MAX_FRAGMENTS

struct ABuffer
{
	uint color;
	float depth;
	uint next;
};

layout(std430, binding = 5) buffer OITBuffer
{
	ABuffer nodes[];
};

layout(binding = 0, r32ui) uniform uimage2D indexPtrs;
layout(binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;

uint PackColor(in vec4 color)
{
	return packUnorm4x8(color);
}

vec4 UnpackColor(uint color)
{
	return unpackUnorm4x8(color);
}

void InsertABufferFragment(in ivec2 fragCoord, in vec4 color, in float depth)
{
	uint nodeindex = atomicCounterIncrement(nextNodeCounter);

	if (nodeindex >= MAX_NODES) return;

	uint prevHead = imageAtomicExchange(indexPtrs, fragCoord.xy, nodeindex);

	nodes[nodeindex].next = prevHead;
	nodes[nodeindex].color = PackColor(color);
	nodes[nodeindex].depth = depth;	
}

void BlendABuffer(in ivec2 fragCoord, inout vec3 color, in float depth)
{
	ABuffer frags[MAX_FRAGMENTS];

	int count = 0;

	uint n = imageLoad(indexPtrs, fragCoord).r;

	if (n == 0xffffffff) return;

	while (n != 0xffffffff && count < MAX_FRAGMENTS)
	{
		frags[count] = nodes[n];
		n = frags[count].next;
		count++;
	}

	for (uint i = 1; i < count; i++) //insert sort
	{
		ABuffer toInsert = frags[i];
		uint j = i;
		while (j > 0 && toInsert.depth > frags[j - 1].depth) {
			frags[j] = frags[j - 1];
			j--;
		}
		frags[j] = toInsert;
	}

	for (int i = 0; i < count; i++)
	{
		vec4 aBuffColor = UnpackColor(frags[i].color);
		color = mix(color, aBuffColor.rgb, aBuffColor.a);
	}
}

/*
vec3 WorldPosFromDepth(float depth) {
	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(TexCoord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = projMatrixInv * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	vec4 worldSpacePosition = viewMatrixInv * viewSpacePosition;

	return worldSpacePosition.xyz;
}
*/