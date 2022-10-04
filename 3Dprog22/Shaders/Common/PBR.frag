
#define PI 3.1459

///Source: https://learnopengl.com/PBR/Lighting

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.f - F0) * pow(clamp(1.f - cosTheta, 0.f, 1.f), 5.f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a*a;
	float NdH = max(dot(N, H), 0.f);
	float NdH2 =  NdH * NdH;

	float num = a2;
	float denom = (NdH2 * (a2 - 1.f)+1.f);

	denom = PI * denom * denom;

	return num / denom;
}

float GeoSchlickGGX(float NdV, float roughness)
{
	float r = (roughness + 1.f);
	float k = (r* r) / 8.f;
	
	float num = NdV;
	float denom = NdV * (1.f - k) + k ;
	return num / denom;
}

float GeoSmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdV = max(dot(N,V), 0.f);
	float NdL = max(dot(N, L), 0.f);
	float ggx1 = GeoSchlickGGX(NdL, roughness);
	float ggx2 = GeoSchlickGGX(NdV, roughness);
	
	return ggx1 * ggx2;
}

vec3 PBR(
	in vec3 N, 
	in vec3 V, 
	in vec3 L,
	in vec3 albedo, 
	float metallic, 
	float roughness, 
	float ssao, 
	float ao,
	in vec3 lightAmbient, 
	in vec3 lightDiffuse, 
	in vec3 lightSpecular, 
	float lightIntensity, 
	float shadowfactor,
	float specFactor = 1.0)
{

	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);

	vec3 H = normalize(V + L);
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeoSmith(N, V, L, roughness);
	vec3 F = FresnelSchlick(clamp(dot(H, V), 0.f, 1.f), F0);

	vec3 kS = F;
	vec3 kD = 1.f.xxx - kS;
	kD *= 1.f - metallic;

	vec3 num = NDF * G * F;
	float denom = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001f;

	vec3 spec = num / denom * specFactor;

	float NdL = max(dot(N, L), 0.f);

	vec3 lo = (kD * albedo / PI + spec) * lightDiffuse * NdL * lightIntensity * 4.f * shadowfactor;

	vec3 ambient = lightAmbient * albedo * ssao * ao;
	vec3 color = ambient + lo;

	return color;
}