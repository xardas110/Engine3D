//from https://www.shadertoy.com/view/wtKyR1

vec4 b127_ieee754_encode(uvec4 v) {
    v.w &= 0x7FFFFFFFu;
    uvec4 l = uvec4(v.xyz & 0xFFFFFFu, v.x >> 24 | v.y >> 24 << 8 | v.z >> 24 << 16);
    return uintBitsToFloat((l & 0x800000u) << 8 | l & 0x7FFFFFu
        | 1u + uvec4(v.w / uvec4(1, 254, 254 * 254, 254 * 254 * 254)) % 254u << 23);
}

uvec4 b127_ieee754_decode(vec4 f) {
    uvec4 e = floatBitsToUint(f);
    uvec4 l = (e & 0x80000000u) >> 8 | e & 0x7FFFFFu;
    uvec4 h = (e >> 23 & 0xFFu) - 1u;
    return uvec4(
        l.xyz | uvec3(l.w) >> uvec3(0, 8, 16) << 24,
        h.x + 254u * h.y + (254u * 254u) * h.z + (254u * 254u * 254u) * h.w);
}


// UINT PACKING 
//8bit number packing 
uint encode4to1(uvec4 x)
{
    x = min(x, uvec4(1 << 8) - 1u);
    return x.w << 24 | x.z << 16 | x.y << 8 | x.x;
}

uvec4 decode1to4(uint x)
{
    return (uvec4(x) >> uvec4(0, 8, 16, 24)) & (uvec4(1 << 8) - 1u);
}

//11 11 10 bit number packing
uint encode3to1(uvec3 x)
{
    x = min(x, uvec3(1) << uvec3(11, 11, 10) - 1u);
    return x.z << 22 | x.y << 11 | x.x;
}

uvec3 decode1to3(uint x)
{
    return (uvec3(x) >> uvec3(0, 11, 22)) & ((uvec3(1) << uvec3(11, 11, 10)) - 1u);
}

//16bit number packing 
uint encode2to1(uvec2 x)
{
    x = min(x, uvec2(1 << 16) - 1u);
    return x.y << 16 | x.x;
}

uvec2 decode1to2(uint x)
{
    return (uvec2(x) >> uvec2(0, 16)) & (uvec2(1 << 16) - 1u);
}


//UINT TO FLOAT CONVERSION 

//uniform quantization
uint uquant(float x, vec2 range, int bits)
{
    return uint(exp2(float(bits)) * ((x)-range.x) / (range.y - range.x));
}

//decode
float ufloat(uint x, vec2 range, int bits)
{
    return float(x) * (range.y - range.x) / exp2(float(bits)) + range.x;
}


//signed logarithmic quantization
//to emulate floating point numbers
uint slogquant(float x, vec2 range, int bits)
{
    uint value = uint(exp2(float(bits - 1)) * (log2(abs(x)) - range.x) / (range.y - range.x));
    uint sig = uint(exp2(float(bits - 1)) * sign(x));
    return value + sig;
}

//decode
float slogfloat(uint x, vec2 range, int bits)
{
    uint value = x & ((1u << (bits - 1)) - 1u);
    uint sig = x >> (bits - 1);
    return (float(sig) * 2.0 - 1.0) * exp2(float(value) * (range.y - range.x) / exp2(float(bits - 1)) + range.x);
}

//unsigned logarithmic quantization
uint logquant(float x, vec2 range, int bits)
{
    return uint(exp2(float(bits)) * (log2(abs(x)) - range.x) / (range.y - range.x));
}

//decode
float logfloat(uint x, vec2 range, int bits)
{
    return exp2(float(x) * (range.y - range.x) / exp2(float(bits)) + range.x);
}

//fibonacci uniform sphere quantization
//to store normals and other vector stuff  


//from https://www.shadertoy.com/view/lllXz4
const float PI = 3.14159265359;
const float PHI = 1.61803398875;

#define M_PI 3.141592653

vec2 fibonacci_lattice(int i, float N)
{
    return vec2((float(i) + 0.5) / N, mod(float(i) / PHI, 1.));
}

vec3 fibonacciDecode(int i, float N)
{
    vec2 xy = fibonacci_lattice(i, N);
    vec2 pt = vec2(2. * M_PI * xy.y, acos(2. * xy.x - 1.) - M_PI * 0.5);
    return vec3(cos(pt.x) * cos(pt.y), sin(pt.x) * cos(pt.y), sin(pt.y));
}

vec2 fibonacciEncode(vec3 p, float n)
{
    float m = 1.0 - 1.0 / n;

    float phi = min(atan(p.y, p.x), PI), cosTheta = p.z;

    float k = max(2.0, floor(log(n * PI * sqrt(5.0) * (1.0 - cosTheta * cosTheta)) / log(PHI + 1.0)));
    float Fk = pow(PHI, k) / sqrt(5.0);
    vec2  F = vec2(round(Fk), round(Fk * PHI)); // k, k+1

    vec2 ka = 2.0 * F / n;
    vec2 kb = 2.0 * PI * (fract((F + 1.0) * PHI) - (PHI - 1.0));

    mat2 iB = mat2(ka.y, -ka.x,
        kb.y, -kb.x) / (ka.y * kb.x - ka.x * kb.y);

    vec2 c = floor(iB * vec2(phi, cosTheta - m));
    float d = 8.0;
    float j = 0.0;
    for (int s = 0; s < 4; s++)
    {
        vec2 uv = vec2(float(s - 2 * (s / 2)), float(s / 2));

        float i = dot(F, uv + c); // all quantities are ingeters (can take a round() for extra safety)

        float phi = 2.0 * PI * fract(i * PHI);
        float cosTheta = m - 2.0 * i / n;
        float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        vec3 q = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
        float squaredDistance = dot(q - p, q - p);
        if (squaredDistance < d)
        {
            d = squaredDistance;
            j = i;
        }
    }
    return vec2(j, sqrt(d));
}


//final packing functions

float PACK4U(vec4 x, vec2 range)
{
    //quantize
    uvec4 quant = uvec4(uquant(x.x, range, 8),
        uquant(x.y, range, 8),
        uquant(x.z, range, 8),
        uquant(x.w, range, 8));
    //pack
    return uintBitsToFloat(encode4to1(quant));
}

vec4 UNPACK4U(float x, vec2 range)
{
    uvec4 unpack = decode1to4(floatBitsToUint(x));

    //unquantize
    return vec4(ufloat(unpack.x, range, 8),
        ufloat(unpack.y, range, 8),
        ufloat(unpack.z, range, 8),
        ufloat(unpack.w, range, 8));
}

float PACK3U(vec3 x, vec2 range)
{
    //quantize
    uvec3 quant = uvec3(uquant(x.x, range, 11),
        uquant(x.y, range, 11),
        uquant(x.z, range, 10));
    //pack
    return uintBitsToFloat(encode3to1(quant));
}

vec3 UNPACK3U(float x, vec2 range)
{
    uvec3 unpack = decode1to3(floatBitsToUint(x));

    //unquantize
    return vec3(ufloat(unpack.x, range, 11),
        ufloat(unpack.y, range, 11),
        ufloat(unpack.z, range, 10));
}

float PACK2U(vec2 x, vec2 range)
{
    //quantize
    uvec2 quant = uvec2(uquant(x.x, range, 16),
        uquant(x.y, range, 16));
    //pack
    return uintBitsToFloat(encode2to1(quant));
}

vec2 UNPACK2U(float x, vec2 range)
{
    uvec2 unpack = decode1to2(floatBitsToUint(x));

    //unquantize
    return vec2(ufloat(unpack.x, range, 16),
        ufloat(unpack.y, range, 16));
}


float PACK4LOG(vec4 x, vec2 range)
{
    //quantize
    uvec4 quant = uvec4(logquant(x.x, range, 8),
        logquant(x.y, range, 8),
        logquant(x.z, range, 8),
        logquant(x.w, range, 8));
    //pack
    return uintBitsToFloat(encode4to1(quant));
}

vec4 UNPACK4LOG(float x, vec2 range)
{
    uvec4 unpack = decode1to4(floatBitsToUint(x));

    //unquantize
    return vec4(logfloat(unpack.x, range, 8),
        logfloat(unpack.y, range, 8),
        logfloat(unpack.z, range, 8),
        logfloat(unpack.w, range, 8));
}

float PACK2LOG(vec2 x, vec2 range)
{
    //quantize
    uvec2 quant = uvec2(logquant(x.x, range, 16),
        logquant(x.y, range, 16));
    //pack
    return uintBitsToFloat(encode2to1(quant));
}

vec2 UNPACK2LOG(float x, vec2 range)
{
    uvec2 unpack = decode1to2(floatBitsToUint(x));

    //unquantize
    return vec2(logfloat(unpack.x, range, 16),
        logfloat(unpack.y, range, 16));
}

//signed log packing
float PACK2SLOG(vec2 x, vec2 range)
{
    //quantize
    uvec2 quant = uvec2(slogquant(x.x, range, 16),
        slogquant(x.y, range, 16));
    //pack
    return uintBitsToFloat(encode2to1(quant));
}

vec2 UNPACK2SLOG(float x, vec2 range)
{
    uvec2 unpack = decode1to2(floatBitsToUint(x));

    //unquantize
    return vec2(slogfloat(unpack.x, range, 16),
        slogfloat(unpack.y, range, 16));
}


//FIBONACCI VECTOR PACKING 
//range here is length range
float PACKVEC3(vec3 x, vec2 range)
{
    float L = length(x);
    vec3 N = normalize(x);

    vec2 fe = fibonacciEncode(N, exp2(16.0));
    //quantize
    uvec2 quant = uvec2(fe.x, logquant(L, range, 16));
    //pack
    return uintBitsToFloat(encode2to1(quant));
}

vec3 UNPACKVEC3(float x, vec2 range)
{
    uvec2 unpack = decode1to2(floatBitsToUint(x));

    //unquantize
    vec3 N = fibonacciDecode(int(unpack.x), exp2(16.0));

    return N * logfloat(unpack.y, range, 16);
}


