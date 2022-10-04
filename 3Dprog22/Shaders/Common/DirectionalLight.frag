
#include "CSM.frag"

struct DirLight 
{
	vec3 ambient;
	float pad1;
	vec3 diffuse;
	float pad2;
	vec3 specular;	
	float pad3;
	vec3 direction;
	float itensity;
};

layout (std140, binding=1) uniform DirlightUBO
{
	DirLight dirlight;
};

vec3 CalcDirLight(DirLight light, vec3 viewDir, vec3 normal, vec3 albedoCol, vec3 specularCol, float shininess, float ssao, float ao, float shadowFactor, float cloudShadow)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	vec3 ambient = light.ambient * albedoCol * dirlight.itensity * ao * ssao;
	vec3 diffuse = light.diffuse * diff * albedoCol * shadowFactor * dirlight.itensity * cloudShadow;
	vec3 specular = light.specular * spec * specularCol* shadowFactor * dirlight.itensity * cloudShadow;

	return (ambient + diffuse + specular);
}

float CalcDirLightShadow(DirLight light, vec3 fragPos, vec3 norm, in mat4 view)
{
	vec4 frag_lightSpace = view * vec4(fragPos, 1.f);
	int level = 3; float depth = abs(frag_lightSpace.z);
	for (int i = 0; i< MAX_CASCADES; i++) //best cascade will always be the first
	{
		if (depth < cascades[i].cascadeOffsetX)
		{
			level = i;
			break;
		}
	}
	frag_lightSpace = cascades[level].cascadeProjection *  vec4(fragPos, 1.f);

	vec3 UVD = vec3(
		frag_lightSpace.x,
		frag_lightSpace.y,
		frag_lightSpace.z);
	UVD.xyz = UVD.xyz * 0.5f + 0.5f;

	//Currently my shadow depth bias don't hold for everything, might find a better solution later if needed.
	float NdL = clamp(dot(norm, -light.direction), 0.f, 1.f);
	float shadowBias = clamp(SHADOW_DEPTH_BIAS * tan(acos(NdL)), 0.f, 0.001f);

	float fragDepth = UVD.z - shadowBias;
 
	const vec2 viewPortDims = vec2(textureSize(shadowMaps, 0));

	const float Dilation = 2.0;
	vec2 texelUnits = 1.f / viewPortDims;
	float d1 = Dilation * texelUnits.x * 0.125;
	float d2 = Dilation * texelUnits.x * 0.875;
	float d3 = Dilation * texelUnits.x * 0.625;
	float d4 = Dilation * texelUnits.x * 0.375;

	//For some reason opengl doesn't return the cmp level, so I have to do the comparison, 
	//in directX the value returned is the comparison boolean and its hardware accelerated.
	float result = 0.f;	
	result +=	texture(shadowMaps, vec3(UVD.xy, level)).r					< fragDepth ? 0.f : 2.f;	 
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(-d2, d1), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(-d1, -d2), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(d2, -d1), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(d1, d2), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(-d4, d3), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(-d3, -d4), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(d4, -d3), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(shadowMaps, vec3(UVD.xy + vec2(d3, d4), level)).r	< fragDepth ? 0.f : 1.f;
	
	result /= 10.f;

	return result * result;	 
}