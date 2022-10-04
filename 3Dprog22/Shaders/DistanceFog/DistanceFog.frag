float GetFogFactor(float dist, float sightRange, float zFar)
{
	return -0.0002f/sightRange*(dist-zFar/10.f*sightRange) + 1.;
}

vec4 CalculateDistanceFog(in vec4 color, in vec3 fogColor, float dist, float sightRange, float zFar)
{
	float fogFactor = GetFogFactor(dist, sightRange, zFar);
	return vec4(mix(fogColor, color.rgb, clamp(fogFactor, 0.f, 1.f)), color.a);
}