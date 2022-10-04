
struct RenderSettings //Due to ubo restrictions bools need to be ints
{
	int fogEnabled;
	float fogSight;
	int volumetricLightEnabled;
	int giEnabled;

	vec3 fogColor;
	int pad3;

	vec3 bloomThreshold;
	int bloomEnabled;

	int lightModel;
	int ssaoEnabled;
	int pad11;
	int pad12;
};

layout(std140, binding=4) uniform renderSettingsUBO
{
	RenderSettings renderSettings;
};