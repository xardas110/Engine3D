#version 460 core
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//
// This is the implementation for my article "improved texture interpolation"
// 
// http://www.iquilezles.org/www/articles/texture/texture.htm
//
// It shows how to get some smooth texture interpolation without resorting to the regular
// bicubic filtering, which is pretty expensive because it needs 9 texels instead of the 
// 4 the hardware uses for bilinear interpolation.
//
// With this techinique here, you van get smooth interpolation while the hardware still 
// uses only 4 texels, by tricking the hardware. The idea is to get the fractional part
// of the texel coordinates and apply a smooth curve to it such that the derivatives are
// zero at the extremes. The regular cubic or quintic smoothstep functions are just
// perfect for this task.

// Modified to do bicubic filtering, sgreen/2013

// Adapted for Processing by Raphaël de Courville <@sableRaph>

uniform sampler2D source;
uniform vec2 targetDim;

vec4 texture2D_smooth(sampler2D tex, vec2 uv, vec2 res)
{
	uv = uv*res + 0.5;
	vec2 iuv = floor( uv );
	vec2 fuv = fract( uv );
	uv = iuv + fuv*fuv*(3.0-2.0*fuv); // fuv*fuv*fuv*(fuv*(fuv*6.0-15.0)+10.0);;
	uv = (uv - 0.5)/res;
	return texture2D( source, uv );
}

// 4x4 bicubic filter using 4 bilinear texture lookups 
// See GPU Gems 2: "Fast Third-Order Texture Filtering", Sigg & Hadwiger:
// http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter20.html

// w0, w1, w2, and w3 are the four cubic B-spline basis functions
float w0(float a)
{
    return (1.0/6.0)*(a*(a*(-a + 3.0) - 3.0) + 1.0);
}

float w1(float a)
{
    return (1.0/6.0)*(a*a*(3.0*a - 6.0) + 4.0);
}

float w2(float a)
{
    return (1.0/6.0)*(a*(a*(-3.0*a + 3.0) + 3.0) + 1.0);
}

float w3(float a)
{
    return (1.0/6.0)*(a*a*a);
}

// g0 and g1 are the two amplitude functions
float g0(float a)
{
    return w0(a) + w1(a);
}

float g1(float a)
{
    return w2(a) + w3(a);
}

// h0 and h1 are the two offset functions
float h0(float a)
{
    return -1.0 + w1(a) / (w0(a) + w1(a));
}

float h1(float a)
{
    return 1.0 + w3(a) / (w2(a) + w3(a));
}

vec4 texture2D_bicubic(sampler2D tex, vec2 uv, vec2 res)
{
	uv = uv*res + 0.5;
	vec2 iuv = floor( uv );
	vec2 fuv = fract( uv );

    float g0x = g0(fuv.x);
    float g1x = g1(fuv.x);
    float h0x = h0(fuv.x);
    float h1x = h1(fuv.x);
    float h0y = h0(fuv.y);
    float h1y = h1(fuv.y);

	vec2 p0 = (vec2(iuv.x + h0x, iuv.y + h0y) - 0.5) / res;
	vec2 p1 = (vec2(iuv.x + h1x, iuv.y + h0y) - 0.5) / res;
	vec2 p2 = (vec2(iuv.x + h0x, iuv.y + h1y) - 0.5) / res;
	vec2 p3 = (vec2(iuv.x + h1x, iuv.y + h1y) - 0.5) / res;
	
    return g0(fuv.y) * (g0x * texture2D(tex, p0)  +
                        g1x * texture2D(tex, p1)) +
           g1(fuv.y) * (g0x * texture2D(tex, p2)  +
                        g1x * texture2D(tex, p3));
}

const int samples = 4; //Square this number to get supersample level

vec4 samplePixel(vec2 uv) {
    // Put all your actual drawing code in here
    vec2 aspect = vec2(targetDim.x/targetDim.y, 1.0);
    uv *= aspect;
    
    float k = step(0.5, texture(source, uv).r);
    return mix(vec4(0.08, 0.15, 0.22, 1.0), vec4(1.00, 0.96, 0.91, 1.0), k);
}

vec4 superSamplePixel(vec2 pos) {
    vec2 off = 1.0 / float(samples) / targetDim.xy;
    vec4 sum = vec4(0.0);
    for (int x=0; x<samples; x++) {
        for (int y=0; y<samples; y++) {
            sum += samplePixel(pos + vec2(off.x*float(x), off.y*float(y)));
        }
    }
    return sum / float(samples * samples);
}

in vec2 TexCoords;
out vec4 FragColor;

void main(void)
{
	const ivec2 sourceDim = ivec2(textureSize(source, 0));
	//ivec2 targetPixel = ivec2(TexCoords * targetDim);

    vec2 uv = gl_FragCoord.xy / targetDim.xy;

	vec3 col = texture2D_bicubic( source, uv, sourceDim ).xyz;	

	//imageStore(target, targetPixel, vec4( col, 1.0 ));
	//discard;
	FragColor = vec4( col, 1.0 );
}
