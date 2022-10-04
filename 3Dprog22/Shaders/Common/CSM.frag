#define MAX_CASCADES 4
#define SHADOW_DEPTH_BIAS 0.00005

struct Cascade
{
	mat4 cascadeProjection;
	float cascadeOffsetX;
	float cascadeOffsetY;
	float cascadeScaleX;
	float cascadeScaleY;
};

layout (std140, binding=0) uniform CSMMatrices
{
	mat4 toShadowSpaceMat;
	Cascade cascades[MAX_CASCADES];
};

uniform sampler2DArray shadowMaps;