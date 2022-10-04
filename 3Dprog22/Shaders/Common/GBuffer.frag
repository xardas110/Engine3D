
struct GBuffer
{
	usampler2D b0; //gBuffer.b0
	sampler2D depth; //gbuffer.depth
};

struct FragInfo_t
{
//------------------ b0.x
	vec3 albedo;
	float specPower;
//------------------
//------------------ b0.y
	vec3 specular;
	uint bitfield;	
//------------------
//------------------ b.z
	vec3 normal;
//------------------
//------------------ b.w
//normal first 16 bytes, rough metallic 16 bytes
	float ao;
	float roughness;
	float metallic;
//------------------
};

uniform GBuffer gBuffer;

uint EncodeNormal(in vec3 n)
{
	return packUnorm2x16(n.xy * 0.5 + 0.5);
}

void DecodeNormal(uint enc, inout vec3 normal)
{
	vec2 n = unpackUnorm2x16(enc);
	normal.xy = n * 2.f - 1.f;
	normal.z = sqrt(1 - dot(normal.xy, normal.xy));
	normal = normalize(normal);
};

uvec4 PackGBuffer(
	in vec3 albedo, 
	float specPower,
	in vec3 specular,
	uint shaderModel,
	in vec3 normal,
	float ao,
	float roughness,
	float metallic
)
{
	uvec4 result;
	result.x = packUnorm4x8(vec4(albedo.xyz, specPower));
	result.y = packUnorm4x8(vec4(specular.rgb, float(shaderModel)/255.f));
	result.z = packHalf2x16(normal.xy);
	result.w = packSnorm4x8(vec4(normal.z, ao, roughness, metallic));
	return result;
}

void UnpackGBuffer(
	in vec2 texCoords,
	inout vec3 albedo,
	inout float specPower,
	inout vec3 specular,
	inout uint bitfield,
	inout vec3 normal,
	inout float ao,
	inout float roughness,
	inout float metallic)
{
	ivec2 size = textureSize(gBuffer.b0, 0);
	uvec4 enc = texture(gBuffer.b0, texCoords);
	
	vec4 x = unpackUnorm4x8(enc.x);
	vec4 y = unpackUnorm4x8(enc.y);
	vec2 z = unpackHalf2x16(enc.z);
	vec4 w = unpackSnorm4x8(enc.w);

	albedo = x.xyz;
	specPower = x.w;

	specular = y.xyz;
	bitfield = uint(y.w * 255.f);
	
	normal = vec3(z, w.x); normal = normalize(normal);

	ao = w.y * 0.5f + 0.5f;
	roughness = w.z * 0.5f + 0.5f;
	metallic = w.w * 0.5f + 0.5f;
}

FragInfo_t UnpackGBuffer(
	in vec2 texCoords)
{
	FragInfo_t fi;
	UnpackGBuffer(texCoords, fi.albedo, fi.specPower, fi.specular, fi.bitfield, fi.normal, fi.ao, fi.roughness, fi.metallic);
	return fi;
}


vec3 WorldPosFromDepth(in vec2 texCoords, in mat4 iProj, in mat4 iView) {
	
	float depth = texture(gBuffer.depth, texCoords).r;

	float z = depth * 2.0 - 1.0;

	vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = iProj * clipSpacePosition;

	// Perspective division
	viewSpacePosition /= viewSpacePosition.w;

	vec4 worldSpacePosition = iView * viewSpacePosition;

	return worldSpacePosition.xyz;
}