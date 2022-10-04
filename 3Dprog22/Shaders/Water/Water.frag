#version 460 core

layout(early_fragment_tests) in;

layout(location = 0) out uvec4 b0Out;

#include "../Common/Camera.frag"
#include "../Common/DirectionalLight.frag"
#include "../Common/GBuffer.frag"
#include "../Common/ShaderModelDefines.frag"

in vec2 TexCoords;
in vec3 Normal;
in vec4 ClipCoords;
in vec4 Position;

uniform float moveFactor;

uniform sampler2D waterDUDV;
uniform sampler2D normalmap;

const float distFactor = 0.05f;

float Random3D(in vec3 st)
{
	return fract(sin(dot(st.xyz, vec3(12.9898, 78.233, 141.1525))) * 43758.5453123);
}

float Interpolate(float a, float b, float x) 
{
	float ft = x * 3.1415927f;
	float f = (1. - cos(ft)) * 0.5;
	return  a * (1. - f) + b * f;
}

float InterpolatedNoise(int ind, float x, float y, float z) {
	int integer_X = int(floor(x));
	float fractional_X = fract(x);
	int integer_Y = int(floor(y));
	float fractional_Y = fract(y);
    int integer_Z = int(floor(z));
    float fractional_Z = fract(z);
    
	vec3 randomInput = vec3(integer_X, integer_Y, integer_Z);
	float v1 = Random3D(randomInput + vec3(0.0, 0.0, 0.0));
	float v2 = Random3D(randomInput + vec3(1.0, 0.0, 0.0));
	float v3 = Random3D(randomInput + vec3(0.0, 1.0, 0.0));
	float v4 = Random3D(randomInput + vec3(1.0, 1.0, 0.0));
    
    float v5 = Random3D(randomInput + vec3(0.0, 0.0, 1.0));
	float v6 = Random3D(randomInput + vec3(1.0, 0.0, 1.0));
	float v7 = Random3D(randomInput + vec3(0.0, 1.0, 1.0));
	float v8 = Random3D(randomInput + vec3(1.0, 1.0, 1.0));
    
    
	float i1 = Interpolate(v1, v2, fractional_X);
	float i2 = Interpolate(v3, v4,  fractional_X);
    
    float i3 = Interpolate(v5, v6, fractional_X);
    float i4 = Interpolate(v7, v8, fractional_X);
    
    float y1 = Interpolate(i1, i2, fractional_Y);
    float y2 = Interpolate(i3, i4, fractional_Y);
    
    
	return Interpolate(y1, y2, fractional_Z);
}

float perlin(float x, float y, float z){
	
    int numOctaves = 3;
	float persistence = 0.5;
	float total = 0.,
		frequency = 0.025,
		amplitude = 6.0;
	for (int i = 0; i < numOctaves; ++i) {
		frequency *= 2.;
		amplitude *= persistence;
		
		x+=moveFactor*13.0;
		y+=moveFactor*7.0;

		total += InterpolatedNoise(0, x * frequency, y * frequency, z * frequency) * amplitude;
		//total += InterpolatedNoise(0, y * frequency, x * frequency, (z+0.5) * frequency) * amplitude;
	}
	return pow(total, 1.0);
}

vec3 computeNormals(vec3 WorldPos){
	float st = 0.35;
	float dhdu = (perlin((WorldPos.x + st), WorldPos.z, moveFactor*10.0) - perlin((WorldPos.x - st), WorldPos.z, moveFactor*10.0))/(2.0*st);
	float dhdv = (perlin( WorldPos.x, (WorldPos.z + st), moveFactor*10.0) - perlin(WorldPos.x, (WorldPos.z - st), moveFactor*10.0))/(2.0*st);

	vec3 X = vec3(1.0, dhdu, 1.0);
	vec3 Z = vec3(0.0, dhdv, 1.0);

	vec3 n = normalize(cross(Z,X));
	vec3 norm = mix(n, Normal, 0.5); 
	norm = normalize(norm);
	return norm;
}

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1,311.7, 194.1729)),dot(p,vec3(269.5,183.3, 72.0192)), dot(p,vec3(183.3,72.0192,311.7))))*43758.5453);
}

float worley(vec3 st) {
	float color = 0.0;

    // Scale
    st *= 0.5;

    // Tile the space
    vec3 i_st = floor(st);
    vec3 f_st = fract(st);

    float m_dist = 1.;  // minimun distance

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
		for (int z = -1; z<=1 ; z++) {

            // Neighbor place in the grid
            vec3 neighbor = vec3(float(x),float(y), float(z));

            // Random position from current + neighbor place in the grid
            vec3 point = random3(i_st + neighbor);


			// Vector between the pixel and the point
            vec3 diff = neighbor + point - f_st;

            // Distance to the point
            float dist = pow(length(diff), 1.0);

            // Keep the closer distance
            m_dist = min(m_dist, dist);
			}
        }
    }

    // Draw the min distance (distance field)
    color += m_dist;

    return color;
}

#define SATURATE(X) clamp(X, 0.0, 1.0)

void main()
{
	float grain = 25.0;

	vec3 oceanColor = vec3(0.5f);

	float distFromPos = distance(Position.xyz, viewPos); 

	vec2 ndc = (ClipCoords.xy/ClipCoords.w)/2.0 + 0.5;
	vec2 distortion1 = texture(waterDUDV, vec2(TexCoords.x + moveFactor, TexCoords.y)*grain).rg*2.0 - 1.0;
	vec2 distortion2 = texture(waterDUDV, vec2(TexCoords.x + moveFactor, TexCoords.y - moveFactor)*grain).rg*2.0 - 1.0;
	vec2 totalDistortion = distortion1 + distortion2;
	
	totalDistortion = normalize(totalDistortion);
	vec3 X = vec3(1.0, totalDistortion.r, 1.0);
	vec3 Z = vec3(0.0, totalDistortion.g, 1.0);

	vec3 norm = texture(normalmap, totalDistortion).rgb;
	norm = vec3(norm.r*2 - 1, norm.b*1.5, norm.g*2 - 1);
	norm = computeNormals(Position.xyz);
	norm = mix(norm, vec3(0.0, 1.0, 0.0), 0.25);

	vec3 lightDir = normalize(-dirlight.direction);
	float diffuseFactor = max(0.0, dot(lightDir, norm.rgb));
	vec3 diffuse = diffuseFactor*dirlight.diffuse * oceanColor;

	float specularFactor = 1.5f;
	vec3 viewDir = normalize(viewPos - Position.xyz);
	vec3 reflectDir = reflect(-lightDir,  normalize(mix(norm, normalize(viewDir*0.8 + vec3(0.0, 1.0, 0.0)), 0.2)) );  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512.);
	vec3 specular = spec * dirlight.specular * specularFactor;  

	vec3 color = diffuse + spec;
	
	b0Out = PackGBuffer(
		color, 
		512,
		specular,
		SHADERMODEL_WATER,
		norm,
		1.f,
		0.f,
		0.f
	);
}