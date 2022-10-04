#version 460 core
out vec4 dofColorOut;

#define DOF_UC_SIZE 16

#define GOLDEN_ANGLE 2.39996323

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform vec2 targetDim;
uniform vec2 uc[DOF_UC_SIZE];

uniform float zNear = 0.1f;
uniform float zFar = 2000.f;

uniform float fov = 90.f;
uniform float focusScale = 50.f; 
uniform float uFar = 0.8f;
uniform float maxBlurSize = 5.f;
uniform float radScale = 0.5f;

in vec2 TexCoords;

float LinearDepth(float depth);
float GetBlurSize(float depth, float focusPoint, float focusScale);
vec3 DepthOfField(vec2 texCoord, float focusPoint, float focusScale);

void main(void)
{
	vec2 uv = TexCoords;

    vec3 color = DepthOfField(uv, fov, focusScale);

	dofColorOut = vec4(color, 1.0); 
}

float LinearDepth(float depth)
{
    float result = depth * 2.f -1.f;
    return (2.f * zNear * zFar) / (zFar + zNear - result*(zFar-zNear));
}

float GetBlurSize(float depth, float focusPoint, float focusScale)
{
	float coc = clamp((1.0 / focusPoint - 1.0 / depth)*focusScale, -1.0, 1.0);
    return abs(coc) * maxBlurSize;
}

//
//source: https://www.shadertoy.com/view/lstBDl
// 
// Modifications made by me
//
vec3 DepthOfField(vec2 texCoord, float focusPoint, float focusScale)
{
    vec4 Input;
    Input.rgb = texture(colorTexture, texCoord).rgb;
    Input.a = LinearDepth(texture(depthTexture, texCoord).r);

    float centerDepth = Input.a * uFar;
    float centerSize = GetBlurSize(centerDepth, focusPoint, focusScale);
    vec3 color = Input.rgb;
    float tot = 1.0;
    
    vec2 texelSize = 1.0 / targetDim.xy;

    float radius = radScale;
    for (float ang = 0.0; radius < maxBlurSize; ang += GOLDEN_ANGLE)
    {
        vec2 tc = texCoord + vec2(cos(ang), sin(ang)) * texelSize * radius;
        
        vec4 sampleInput;
        sampleInput.rgb = texture(colorTexture, tc).rgb;
        sampleInput.a = LinearDepth(texture(depthTexture, tc).r);

        vec3 sampleColor = sampleInput.rgb;
        float sampleDepth = sampleInput.a * uFar;
        float sampleSize = GetBlurSize(sampleDepth, focusPoint, focusScale);
        
        if (sampleDepth > centerDepth)
        {
        	sampleSize = clamp(sampleSize, 0.0, centerSize*2.0);
        }

        float m = smoothstep(radius-0.5, radius+0.5, sampleSize);
        color += mix(color/tot, sampleColor, m);
        tot += 1.0;
        radius += radScale/radius;
    }
    
    return color /= tot;
}