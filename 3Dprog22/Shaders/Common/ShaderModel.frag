#include "ShaderModelDefines.frag"
#include "GBuffer.frag"
#include "PBR.frag"

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

vec4 SM_Sun(in vec3 albedo)
{
    return vec4(albedo, 1.f);
}

vec4 SM_Sky(inout vec4 cloudColor, inout vec3 volumetricLight)
{
	return vec4(cloudColor.rgb + volumetricLight, 1.f);
};

vec3 BRDF(DirLight light, vec3 vp, vec3 N, vec3 X, vec3 ka, vec4 ks, float shadowFactor)
{
    // common variables
    vec3 L = -light.direction;
    vec3 V = normalize(vp - X);
    vec3 H = normalize(V + L);
    // compute dot procuts
    float dotNL = max(dot(N, L), 0.0f);
    float dotNH = max(dot(N, H), 0.0f);
    float dotLH = max(dot(L, H), 0.0f);
    // decode specular power
    float spec = exp2(11.0f * ks.a + 1.0f);
    // emulate fresnel effect
    vec3 fresnel = ks.rgb + (1.0f - ks.rgb) * pow(1.0f - dotLH, 5.0f);
    // specular factor
    float blinnPhong = pow(dotNH, spec);
    // energy conservation, aprox normalization factor
    blinnPhong *= spec * 0.0397f + 0.3183f;
    // specular term
    vec3 specular = ks.rgb * light.specular * blinnPhong * fresnel * shadowFactor * light.itensity;
    // diffuse term
    vec3 diffuse = ka.rgb * light.diffuse * shadowFactor * light.itensity;

    vec3 ambient = ka.rgb * light.ambient;
    // return composition
    return ((diffuse + specular) * dotNL) + ambient;
};

vec3 BRDF(DirLight light, vec3 vp, vec3 N, vec3 X, vec3 ka, vec4 ks, float shadowFactor, float ao, float ssao)
{
    // common variables
    vec3 L = -light.direction;
    vec3 V = normalize(vp - X);
    vec3 H = normalize(V + L);
    // compute dot procuts
    float dotNL = max(dot(N, L), 0.0f);
    float dotNH = max(dot(N, H), 0.0f);
    float dotLH = max(dot(L, H), 0.0f);
    // decode specular power
    float spec = exp2(11.0f * ks.a + 1.0f);
    // emulate fresnel effect
    vec3 fresnel = ks.rgb + (1.0f - ks.rgb) * pow(1.0f - dotLH, 5.0f);
    // specular factor
    float blinnPhong = pow(dotNH, spec);
    // energy conservation, aprox normalization factor
    blinnPhong *= spec * 0.0397f + 0.3183f;
    // specular term
    vec3 specular = ks.rgb * light.specular * blinnPhong * fresnel * shadowFactor * light.itensity;
    // diffuse term
    vec3 diffuse = ka.rgb * light.diffuse * shadowFactor * light.itensity;

    vec3 ambient = ka.rgb * light.ambient * ao * ssao;
    // return composition
    return ((diffuse + specular) * dotNL) + ambient;
};

vec3 PHONG(in DirLight light, 
    in vec3 viewDir, 
    in vec3 normal, 
    in vec3 albedoCol, 
    in vec3 specularCol, 
    float shininess, float ssao, 
    float ao, float shadowFactor)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	vec3 ambient = light.ambient * albedoCol * light.itensity * ao * ssao;
	vec3 diffuse = light.diffuse * diff * albedoCol  * shadowFactor * light.itensity;
	vec3 specular = light.specular * spec * specularCol * shadowFactor * light.itensity;
	return (ambient + diffuse + specular);
};

vec4 SM_PBR(
    in DirLight light,
    in vec3 VP,
    in vec3 N,
    in vec3 X,  
	in vec3 albedo, 
	float metallic, 
	float roughness, 
	float ssao, 
    float ao,
	float shadowfactor,
    float specFactor = 1.0)
{ 
    vec3 V = VP - X;
    V = normalize(V);
    vec3 L = normalize(-light.direction);

    vec3 directLighting = PBR(N, 
	    V, 
	    L,
	    albedo, 
	    metallic, 
	    roughness, 
	    ssao, 
        ao,
	    light.ambient, 
	    light.diffuse, 
	    light.specular, 
	    light.itensity, 
	    shadowfactor,
        specFactor);

    return vec4(directLighting, 1.f);
}


vec4 SM_BRDF(in DirLight light, float shadowFactor, in vec3 vp, in vec3 N, in vec3 X, in vec3 ka, in vec4 ks)
{
    vec3 viewDir = vp - X;
    viewDir = normalize(viewDir);

    /*
    vec3 directLighting = PHONG(
        light, 
        viewDir, 
        N, 
        ka, 
        ks.rgb, 
        64.f, 
        1.f, 1.f, 
        shadowFactor);
    */
    vec3 directLighting = BRDF(light, vp, N, X, ka, ks, shadowFactor);

	return vec4(directLighting, 1.f);
};

vec4 SM_BRDF(in DirLight light, float shadowFactor, float ao, float ssao, in vec3 vp, in vec3 N, in vec3 X, in vec3 ka, in vec4 ks)
{
    vec3 viewDir = vp - X;
    viewDir = normalize(viewDir);

    vec3 directLighting = BRDF(light, vp, N, X, ka, ks, shadowFactor, ao, ssao);

	return vec4(directLighting, 1.f);
};

vec4 SM_BRDF_INDIRECT(DirLight light, vec3 volumetricLight, float bounceStrength, vec4 indirectDiffuseAO, vec4 indirectSpecularShadow, float shadowFactor, vec3 vp, vec3 N, vec3 X, vec3 ka, vec4 ks, float ssao)
{
    vec3 directLighting = BRDF(light, vp, N, X, ka, ks, shadowFactor) * light.itensity * ssao;
	vec3 indirectDiffuse = indirectDiffuseAO.rgb * ka;
	vec3 indirectSpecular = indirectSpecularShadow.rgb * ks.rgb;
	vec3 indirectLighting = (indirectDiffuse + indirectSpecular) * light.itensity * bounceStrength;
	return vec4(((directLighting + indirectLighting) * indirectDiffuseAO.a) + volumetricLight, 1.f);
};

vec4 SM_TRANSPARENT(vec4 accum, float reveal)
{
    vec3 col = accum.rgb / max(accum.a, 0.00001f);
    return vec4(col, 1.f - reveal);
};