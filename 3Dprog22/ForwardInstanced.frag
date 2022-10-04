#version 460 core
out vec4 FragColor;
  
  //The shaderparamenter header will replace defines from c++ code. It won't add any new code so the GLSL correction tool won't give syntax errors.

  #define MAX_CASCADES 6
  #define SHADOW_DEPTH_BIAS 0.0002

struct Material {
	sampler2D diffusemap;
	sampler2D specularmap;
	sampler2D normalmap;
	sampler2D opacitymap; 
	sampler2D AOmap; 

	int numNormalmaps;
	int numDiffusemaps;
	int numSpecularmaps;
	int numOpacitymaps;
	int numAOmaps;

	vec3 color; 
	float shininess;
};

struct Cascade
{
	mat4 cascadeProjection;
	float cascadeOffsetX;
	float cascadeOffsetY;
	float cascadeScaleX;
	float cascadeScaleY;
};

///For now only 1 dirlight is supported with shadows, but it will be very easy to dynamically add more
struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;		
};

uniform sampler2DArray smCascades;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;
uniform mat4 camView;

layout (std140, binding=0) uniform CSMMatrices
{
	mat4 toShadowSpaceMat;
	Cascade cascades[MAX_CASCADES];
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
float CalcDirLightShadow(DirLight light, vec3 normal);

float GetAlphaValue();

float GetAO();
vec3 GetDiffuseColor();
vec3 GetSpecularColor();
vec3 GetNormal();

void main()
{
	float alpha = GetAlphaValue();

	vec3 norm = GetNormal();
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	FragColor = vec4(result, alpha);
}

float CalcDirLightShadow(DirLight light, vec3 norm)
{
	vec4 frag_lightSpace = camView * vec4(FragPos, 1.f);
	int level = 3; float depth = abs(frag_lightSpace.z);
	for (int i = 0; i< MAX_CASCADES; i++) //best cascade will always be the first
	{
		if (depth < cascades[i].cascadeOffsetX)
		{
			level = i;
			break;
		}
	}
	frag_lightSpace = cascades[level].cascadeProjection *  vec4(FragPos, 1.f);

	vec3 UVD = vec3(
		frag_lightSpace.x,
		frag_lightSpace.y,
		frag_lightSpace.z);
	UVD.xyz = UVD.xyz * 0.5f + 0.5f;

	//Currently my shadow depth bias don't hold for everything, might find a better solution later if needed.
	float NdL = clamp(dot(norm, -light.direction), 0.f, 1.f);
	float shadowBias = clamp(SHADOW_DEPTH_BIAS * tan(acos(NdL)), 0.f, 0.001f);

	float fragDepth = UVD.z - shadowBias;
 
	const vec2 viewPortDims = vec2(textureSize(smCascades, 0));

	const float Dilation = 2.0;
	vec2 texelUnits = 1.f / viewPortDims;
	float d1 = Dilation * texelUnits.x * 0.125;
	float d2 = Dilation * texelUnits.x * 0.875;
	float d3 = Dilation * texelUnits.x * 0.625;
	float d4 = Dilation * texelUnits.x * 0.375;

	//For some reason opengl doesn't return the cmp level, so I have to do the comparison, 
	//in directX the value returned is the comparison boolean and its hardware accelerated.
	float result = 0.f;	
	result +=	texture(smCascades, vec3(UVD.xy, level)).r					< fragDepth ? 0.f : 2.f;	 
	result +=	texture(smCascades, vec3(UVD.xy + vec2(-d2, d1), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(smCascades, vec3(UVD.xy + vec2(-d1, -d2), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(smCascades, vec3(UVD.xy + vec2(d2, -d1), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(smCascades, vec3(UVD.xy + vec2(d1, d2), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(smCascades, vec3(UVD.xy + vec2(-d4, d3), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(smCascades, vec3(UVD.xy + vec2(-d3, -d4), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(smCascades, vec3(UVD.xy + vec2(d4, -d3), level)).r	< fragDepth ? 0.f : 1.f;
	result +=	texture(smCascades, vec3(UVD.xy + vec2(d3, d4), level)).r	< fragDepth ? 0.f : 1.f;
	
	result /= 10.f;

	return result * result;	 
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float shadowFactor = CalcDirLightShadow(dirLight, normal);

	vec3 ambient = light.ambient * GetDiffuseColor() * material.color;
	vec3 diffuse = light.diffuse * diff * GetDiffuseColor() * GetAO() * material.color * shadowFactor;
	vec3 specular = light.specular * spec * GetSpecularColor() * GetAO() * material.color * shadowFactor;
	return (ambient + diffuse + specular);
}
vec3 GetDiffuseColor() // returns either a texture diffuse color or vec3(1)
{
	if (material.numDiffusemaps != 0)
	{
		return vec3(texture(material.diffusemap, TexCoords));
	}
	return vec3(1.f, 1.f, 1.f);
}
vec3 GetSpecularColor()// returns either a texture specular color or vec3(1)
{
	if (material.numSpecularmaps != 0)
	{
		return vec3(texture(material.specularmap, TexCoords));
	}
	return vec3(1.f, 1.f, 1.f);
}
vec3 GetNormal()
{
	if (material.numNormalmaps != 0)
	{
		vec3 normalMap = texture(material.normalmap, TexCoords).rgb;
		vec3 norm = normalMap * 2.f - 1.f;
		return normalize(TBN * norm);
	}

	return normalize(Normal);
}

float GetAlphaValue()
{
	if (material.numOpacitymaps != 0)
		return texture(material.opacitymap, TexCoords).r;	
	return 1.f;
}

float GetAO()
{
	if (material.numAOmaps != 0)
	{
		return texture(material.AOmap, TexCoords).r;
	}

	return 1.f;
}