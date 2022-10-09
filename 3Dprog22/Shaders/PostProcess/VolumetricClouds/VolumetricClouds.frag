#version 460 core

/// <summary>
/// ============================================
/// GPU PRO 7, see p. 97
/// ============================================
/// Based on GPU PRO 7, NadirRoGue and Federico Vaccaro's versions with many new modifications and improvements.
/// ============================================
/// Brief: 2 3D textures and 1 weather texture
/// ------------------------------------------------------------------------------
/// First 3D texture 128^3 consists of PerlinWorley in the red channel and 3 Worley noises at increasing frequences in GBA channels. 
/// Used to create the base shape of the clouds.
/// Second 3D texture 32^3 WorleyFBMs used as the edges for the base clouds
/// Weather texture is used to lookup cloud coverage and cloud type, precipitation will not be added.
/// ------------------------------------------------------------------------------

layout(location = 0) out vec4 outCloudColor;
layout(location = 1) out vec4 outCloudShadowVL;

#include "../../Common/gBuffer.frag"
#include "../../Common/Camera.frag"
#include "../../Common/RaycastUtility.frag"

#define STRATUS_GRADIENT vec4(0.0, 0.1, 0.2, 0.3)
#define STRATOCUMULUS_GRADIENT vec4(0.02, 0.2, 0.48, 0.625)
#define CUMULUS_GRADIENT vec4(0.00, 0.1625, 0.88, 0.98)
#define CLOUDS_MIN_TRANSMITTANCE 1e-1
#define CLOUDS_TRANSMITTANCE_THRESHOLD 1.0 - CLOUDS_MIN_TRANSMITTANCE
#define CLOUD_TOP_OFFSET 750.0
#define BAYER_FACTOR 1.0/16.0

struct DirLight {
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

struct VolumetricClouds
{
	float crispiness;
	float cloudSpeed;
	float curliness;
	float coverageMultiplier;
	float densityFactor;
	float absorption;
	vec3 cloudColorBot;
	vec3 windDirection;

	float earthRadius;
	float atmosphereInner;
	float atmosphereOuter;

	float vlScattering;
	vec3 vlLightColor;	

	bool bExpensive;

	int numCloudSteps;
	int numShadowSteps;
	int numVolumetricLightSteps;
};

uniform float bayerFilter[16u] = float[]
(
	0.0*BAYER_FACTOR, 8.0*BAYER_FACTOR, 2.0*BAYER_FACTOR, 10.0*BAYER_FACTOR,
	12.0*BAYER_FACTOR, 4.0*BAYER_FACTOR, 14.0*BAYER_FACTOR, 6.0*BAYER_FACTOR,
	3.0*BAYER_FACTOR, 11.0*BAYER_FACTOR, 1.0*BAYER_FACTOR, 9.0*BAYER_FACTOR,
	15.0*BAYER_FACTOR, 7.0*BAYER_FACTOR, 13.0*BAYER_FACTOR, 5.0*BAYER_FACTOR
);

uniform vec3 noiseKernel[6u] = vec3[] 
(
	vec3( 0.38051305,  0.92453449, -0.02111345),
	vec3(-0.50625799, -0.03590792, -0.86163418),
	vec3(-0.32509218, -0.94557439,  0.01428793),
	vec3( 0.09026238, -0.27376545,  0.95755165),
	vec3( 0.28128598,  0.42443639, -0.86065785),
	vec3(-0.16852403,  0.14748697,  0.97460106)
);

uniform sampler3D perlinWorley;
uniform sampler3D worley;
uniform sampler2D weather;
uniform float fTime = 0.0f;
uniform VolumetricClouds vc;

const float vlScattering = vc.vlScattering;
const float vlScattering2 = vlScattering*vlScattering;
const float PIscat = 3.14159;
const float sa = 4.0 * PIscat;

const vec3 atomospherePos = vec3(0., -vc.earthRadius, 0.);
const float atmosphereInner = vc.earthRadius + vc.atmosphereInner;
const float atmosphereOuter = atmosphereInner + vc.atmosphereOuter;

float ComputeScattering(float LdV)
{
	float result = 1.0f - vlScattering2;
	result /= (sa * pow(1.0f + vlScattering2 - (2.0f * vlScattering) * LdV, 1.5f));
	return result;
}

float GetHeightFractionForPoint(in vec3 inPos, in vec3 sphereCenter, in float sphereInner, in float sphereOuter)
{
	return (length(inPos - sphereCenter) - sphereInner)/(sphereOuter - sphereInner);
}

float Remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax)
{
	return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

vec2 GetUVProj(in vec3 p, in float inSphereInner)
{
	return p.xz/inSphereInner + 0.5f;
}

float GetDensity(float heightFraction, float cloudType)
{
	float stratusFactor = 1.0 - clamp(cloudType * 2.0, 0.0, 1.0);
	float stratoCumulusFactor = 1.0 - abs(cloudType - 0.5) * 2.0;
	float cumulusFactor = clamp(cloudType - 0.5, 0.0, 1.0) * 2.0;

	vec4 baseGradient = (
	stratusFactor * STRATUS_GRADIENT + 
	stratoCumulusFactor * STRATOCUMULUS_GRADIENT + 
	cumulusFactor * CUMULUS_GRADIENT);

	return smoothstep(baseGradient.x, baseGradient.y, heightFraction) - smoothstep(baseGradient.z, baseGradient.w, heightFraction);
}

float SampleCloudDensity(
	in vec3 p, 
	in vec3 sphereCenter, 
	in float inSphereInner, 
	in float inSphereOuter, 
	float LOD, 
	float time, 
	sampler3D texA, 
	sampler3D texB, 
	sampler2D texWeather)
{
	float heightFraction = GetHeightFractionForPoint(
		p, 
		sphereCenter, 
		inSphereInner, 
		inSphereOuter);

	vec3 animation = (
		heightFraction * vc.windDirection * CLOUD_TOP_OFFSET + 
		vc.windDirection * time * vc.cloudSpeed);

	vec2 uv = GetUVProj(p, inSphereInner);
	vec2 animatedUV = GetUVProj(p + animation, inSphereInner);

	if(heightFraction < 0.0 || heightFraction > 1.0){
		return 0.0;
	}

	vec4 lowFreqNoise = textureLod(texA, vec3(uv*vc.crispiness, heightFraction), LOD);
	float lowFreqFBM = dot(lowFreqNoise.gba, vec3(0.625, 0.25, 0.125));
	float baseCloud = Remap(lowFreqNoise.r, -(1.0 - lowFreqFBM), 1., 0.0 , 1.0);
	
	float density = GetDensity(heightFraction, 1.0);
	baseCloud *= (density/heightFraction);

	vec3 weatherData = texture(texWeather, animatedUV).rgb;
	float cloudCoverage = weatherData.r*vc.coverageMultiplier;
	float bcwc = Remap(baseCloud , cloudCoverage , 1.0 , 0.0 , 1.0) * cloudCoverage;

	if(vc.bExpensive)
	{
		vec3 erodeCloudNoise = textureLod(texB, vec3(animatedUV*vc.crispiness, heightFraction)*vc.curliness, LOD).rgb;
		float highFreqFBM = dot(erodeCloudNoise.rgb, vec3(0.625, 0.25, 0.125));
		float highFreqNoiseModifier = mix(highFreqFBM, 1.0 - highFreqFBM, clamp(heightFraction * 10.0, 0.0, 1.0));

		bcwc = bcwc - highFreqNoiseModifier * (1.0 - bcwc);
		bcwc = Remap(bcwc*2.0, highFreqNoiseModifier * 0.2, 1.0, 0.0, 1.0);
	}

	return clamp(bcwc, 0.0, 1.0);
}

float Powder(float d)
{
	return (1. - exp(-2.*d));
}

float HG( float sundotrd, float g) 
{
	float gg = g * g;
	return (1. - gg) / pow( 1. + gg - 2. * g * sundotrd, 1.5);
}

float MarchToLight0(
	vec3 sp,
	vec3 ep,
	int numSteps,  
	in vec3 sphereCenter,
	in float sphereInner,
	in float sphereOuter,
	in float time,
	in sampler3D texA,
	in sampler3D texB,
	in sampler2D texWeather
)
{
	vec3 toLight = ep - sp;
	float len = length(toLight);
	vec3 dir = toLight / len;

	float stepSize = len / float(numSteps);

	vec3 pos = sp;

	float sigmaDs = -stepSize*vc.absorption;

	float result = 1.0;
	for(int i = 0; i < numSteps; i++)
	{
		float heightFraction = GetHeightFractionForPoint(pos, sphereCenter, sphereInner, sphereOuter);
		if(heightFraction >= 0)
		{		
			float cloudDensity = SampleCloudDensity(
				pos, 
				sphereCenter, 
				sphereInner, 
				sphereOuter, 
				i/4, 
				time, 
				texA, 
				texB, 
				texWeather);

			if(cloudDensity > 0.0)
			{
				float Ti = exp(cloudDensity*sigmaDs);
				result *= Ti;
			}
		}	
		
		if (result <= 0.0001) return 0.;
		pos += dir * stepSize;
	}

	return result;
}

float GetCloudShadow(
	uvec2 fragCoord,
	vec3 fragWs,
	float time,
	vec3 sunDir,
	in sampler3D texA, 
	in sampler3D texB, 
	in sampler2D texWeather
)
{
	vec3 startPos, endPos;
	bool bResult = RaySphereIntersect0(
		fragWs, 
		sunDir, 
		atomospherePos, 
		atmosphereInner, 
		atmosphereOuter, 
		startPos, 
		endPos);

	if (!bResult) return 1.;

	int numSteps = vc.numShadowSteps;	

	/*Maybe a cheaper method? Was no difference*/
	/*
	vec3 toLight = endPos - startPos;
	float len = length(toLight);
	vec3 dir = toLight / len;
	float stepSize = len/float(numSteps);

	int a = int(fragCoord.x) % 4;
	int b = int(fragCoord.y) % 4;

	startPos += dir * bayerFilter[a * 4 + b];

	vec3 p = startPos;
	float result = 1.;

	for (int i = 0; i < numSteps; i++)
	{
		float heightFraction = GetHeightFractionForPoint(p, atomospherePos, atmosphereInner, atmosphereOuter);
		vec3 animation = heightFraction * vc.windDirection * CLOUD_TOP_OFFSET + vc.windDirection * time * vc.cloudSpeed;
		vec2 uv = GetUVProj(p, atmosphereInner);
		vec2 animatedUV = GetUVProj(p + animation, atmosphereInner);
		float weatherData = texture(texWeather, animatedUV).r;
		float cloudCoverage = weatherData*vc.coverageMultiplier;

		result -= cloudCoverage;

		if (result <= 0.0005) return result;

		p+= dir * stepSize;
	}
*/

	float lightFactor = MarchToLight0(
		startPos,
		endPos,
		numSteps,  
		atomospherePos,
		atmosphereInner,
		atmosphereOuter,
		time,
		texA,
		texB,
		texWeather);
		
	return lightFactor;
}

/*
 x contains shadow
 yzw contains volumetricLight
*/
vec4 GetCloudShadowAndVolumetricLight(
	vec2 texCoords,
	uvec2 fragCoord,
	vec3 camPos,
	vec3 fragWs,
	float time,
	vec3 sunDir,
	vec3 sunColor,
	in sampler3D texA, 
	in sampler3D texB, 
	in sampler2D texWeather
)
{
	vec4 result = vec4(1., 0., 0., 0.);

/*	
	vec3 endPos = camPos;
	vec3 startPos = fragWs;

	int numSteps = vc.numVolumetricLightSteps;
	vec3 toAtomsphere = endPos - startPos;
	float len = length(toAtomsphere);
	vec3 dir = toAtomsphere / len;
	float stepSize = len / float(numSteps);

	float LdV = dot(dir, sunDir);

	vec3 sc = vc.vlLightColor;

	vec3 pos = startPos;
*/

	float shadow = GetCloudShadow(	
			fragCoord,
			fragWs,
			time,
			sunDir,
			texA, 
			texB, 
			texWeather);

	/*
	for(int i = 0; i < numSteps; i++)
	{
		result.yzw += (ComputeScattering(LdV) * sc * shadow);

		pos += dir * stepSize;
	}
	*/
	result.x = shadow;
	return result;
}

float MarchToLight(
	vec3 o, 
	float stepSize, 
	vec3 lightDir, 
	in vec3 sphereCenter,
	in float sphereInner,
	in float sphereOuter,
	in float time,
	in sampler3D texA,
	in sampler3D texB,
	in sampler2D texWeather
)
{
	vec3 startPos = o;
	float ds = stepSize * 6.0;
	vec3 rayStep = lightDir * ds;	
	float coneRadius = 1.0; 
	float density = 0.0;
	float coneDensity = 0.0;
	float invDepth = 1.0/ds;
	float sigmaDs = -ds*vc.absorption;
	const float coneStep = 1.0/6.0;
	vec3 pos;

	float T = 1.0;

	for(int i = 0; i < 6; i++)
	{
		pos = startPos + coneRadius*noiseKernel[i]*float(i);

		float heightFraction = GetHeightFractionForPoint(pos, sphereCenter, sphereInner, sphereOuter);
		if(heightFraction >= 0)
		{
			
			float cloudDensity = SampleCloudDensity(
				pos, 
				sphereCenter, 
				sphereInner, 
				sphereOuter, 
				i/16, 
				time, 
				texA, 
				texB, 
				texWeather);

			if(cloudDensity > 0.0)
			{
				float Ti = exp(cloudDensity*sigmaDs);
				T *= Ti;
				density += cloudDensity;
			}
		}
		startPos += rayStep;
		coneRadius += coneStep;
	}

	return T;
}

vec4 MarchToHeaven(
	vec3 sp, 
	vec3 ep, 
	in vec3 sphereCenter,
	in float sphereInner,
	in float sphereOuter,
	in uvec2 fragCoord, 
	in float time,
	in vec3 bg,
	in vec3 sunDir,
	in vec3 sunColor,
	in sampler3D texA, 
	in sampler3D texB, 
	in sampler2D texWeather)
{
	vec3 path = ep - sp;
	float len = length(path);

	const int nSteps = vc.numCloudSteps;
	
	float ds = len/nSteps;
	vec3 dir = path/len;
	dir *= ds;
	vec4 col = vec4(0.0);
	int a = int(fragCoord.x) % 4;
	int b = int(fragCoord.y) % 4;
	sp += dir * bayerFilter[a * 4 + b];
	vec3 pos = sp;

	float lightDotEye = dot(normalize(sunDir), normalize(dir));

	float density = 0.0;

	float result = 1.0;
	float sigmaDs = -ds*vc.densityFactor;

	for(int i = 0; i < nSteps; ++i)
	{	
		float densitySample = SampleCloudDensity(pos, sphereCenter, sphereInner, sphereOuter, i/16, time, texA, texB, texWeather);
		if(densitySample > 0.)
		{
			float height = GetHeightFractionForPoint(
				pos, 
				sphereCenter, 
				sphereInner, 
				sphereOuter);

			float lightDensity = MarchToLight(
				pos, 
				ds*0.1, 
				sunDir, 
				sphereCenter,
				sphereInner,
				sphereOuter,
				time,
				texA,
				texB,
				texWeather);

			float scattering = mix(
				HG(lightDotEye, -0.08), 
				HG(lightDotEye, 0.08), 
				clamp(lightDotEye*0.5 + 0.5, 
				0.0, 1.0));

			float powderTerm =  Powder(densitySample);
			vec3 S = 0.6*( mix( mix(vc.cloudColorBot*1.8, bg, 0.2), scattering*sunColor, powderTerm*lightDensity)) * densitySample;
			float dTrans = exp(densitySample*sigmaDs);
			vec3 Sint = (S - S * dTrans) * (1. / densitySample);
			col.rgb += result * Sint;
			result *= dTrans;
		}

		if( result <= CLOUDS_MIN_TRANSMITTANCE ) break;

		pos += dir;
	}
	col.a = 1.0 - result;
	return col;
}

vec3 ComputeClipSpaceCoord(in vec2 texCoords){
	vec2 ray_nds = 2.0*texCoords - 1.0;
	return vec3(ray_nds, 1.0);
}

vec3 ComputeWorldDir(in vec2 texCoords, in mat4 invView, in mat4 invProj)
{
	vec4 rayClip = vec4(ComputeClipSpaceCoord(texCoords), 1.f);
	vec4 rayView = invProj * rayClip;
	rayView = vec4(rayView.xy, -1.f, 0.f);
	vec3 worldDir = (invView * rayView).xyz;
	return normalize(worldDir);
}

float Threshold(const float v, const float t)
{
	return v > t ? v : 0.0;
}

float ComputeFogAmount(in vec3 startPos, in float factor, in vec3 cameraPosition, in vec3 sphereCenter, float amp)
{
	float dist = length(startPos - cameraPosition);
	float radius = (cameraPosition.y - sphereCenter.y)*amp;
	float alpha = (dist / radius);
	return (1.-exp( -dist*alpha*factor));
}

vec4 GetCloudColor(
	vec3 cameraPos,
	in mat4 invView,
	in mat4 invProj,
	vec2 texCoords,
	uvec2 fragCoords,
	float time,
	vec3 bg, //Background color
	in vec3 sunDir,
	vec3 sunColor,
	in sampler3D texA, 
	in sampler3D texB, 
	in sampler2D texWeather
)
{
	sunColor *= 3.0;

	vec3 worldDir = ComputeWorldDir(texCoords, invView, invProj);

	if (worldDir.y < -0.2) return vec4(bg, 1.); 

	vec3 startPos, endPos;
	bool bResult = RaySphereIntersect0(
		cameraPos, 
		worldDir, 
		atomospherePos, 
		atmosphereInner, 
		atmosphereOuter, 
		startPos, 
		endPos);

	if (!bResult) return vec4(bg, 1.);

	float fogAmount = ComputeFogAmount(startPos, 0.00006, cameraPos, atomospherePos, 0.05);
	if (fogAmount > 0.97) return vec4(bg, 1.);

	vec4 v = MarchToHeaven(
		startPos, 
		endPos, 
		atomospherePos, 
		atmosphereInner,
		atmosphereOuter, 
		fragCoords, 
		time,
		bg,
		sunDir, 
		sunColor,
		texA, 
		texB, 
		texWeather);

	float cloudAlphaness = Threshold(v.a, 0.2);

	v.rgb = v.rgb*1.8 - 0.1;
	v.rgb = mix(v.rgb, bg.rgb*v.a, clamp(fogAmount,0.,1.));

	float sun = clamp( dot(sunDir,normalize(endPos - startPos)), 0.0, 1.0 );
	vec3 s = 0.8*sunColor*pow( sun, 256.0 );
	v.rgb += s*v.a;

	bg.rgb = bg.rgb*(1.0 - v.a) + v.rgb;

	return vec4(bg.rgb, cloudAlphaness);
}

in vec2 TexCoords;
void main()
{
	FragInfo_t fi = UnpackGBuffer(TexCoords);
	vec3 albedo = fi.albedo; //ToDOFIX!
	vec3 fragPos = WorldPosFromDepth(TexCoords, inverse(proj), inverse(view));
	uvec2 fragCoord = uvec2(gl_FragCoord.xy);
	vec3 invSunDir = normalize(-dirlight.direction);

	vec4 cloudColor = GetCloudColor(
		viewPos,
		inverse(view),
		inverse(proj),
		TexCoords,
		fragCoord,
		fTime,
		albedo, //Background color
		invSunDir,
		dirlight.diffuse,
		perlinWorley, 
		worley, 
		weather);

	vec4 cloudShadowVL = GetCloudShadowAndVolumetricLight(
		TexCoords,
		fragCoord,
		viewPos,
		fragPos,
		fTime,
		invSunDir,
		dirlight.diffuse,
		perlinWorley, 
		worley, 
		weather);

	cloudShadowVL.yzw = vec3(0.);

	outCloudColor = vec4(albedo.rgb*(1.0 - cloudColor.a) + cloudColor.rgb * cloudColor.a, 1.);
	outCloudShadowVL = cloudShadowVL;
}
