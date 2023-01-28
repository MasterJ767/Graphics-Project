#include "Noise.h"
#include <algorithm>

Noise::Noise() {
	p = new int[256]{ 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,
		36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,
		62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,
		171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,
		60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,
		80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,
		109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,
		212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,
		154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,
		232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
		81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184, 84,204,176,115,
		121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,
		215,61,156,180 };
}

Noise::~Noise() {
	delete[] p;
}

float Noise::grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

inline float Noise::mod(float num, int div) {
	return num - div * floor(num / div);
}

Vector4 Noise::mod(Vector4 vec4, int div) {
	return Vector4(mod(vec4.x, div), mod(vec4.y, div), mod(vec4.z, div), mod(vec4.w, div));
}

inline float Noise::fract(float num) {
	return num - floor(num);
}

Vector4 Noise::fract(Vector4 vec4) {
	return Vector4(fract(vec4.x), fract(vec4.y), fract(vec4.z), fract(vec4.w));
}

inline float Noise::permute(float num) {
	return mod(((num * 34) + 1 * num), 289);
}

Vector4 Noise::permute(Vector4 vec4) {
	return Vector4(permute(vec4.x), permute(vec4.y), permute(vec4.z), permute(vec4.w));
}

float Noise::rawNoise(Vector3 point) {
	int xi = (int)point.x & 255;
	int yi = (int)point.y & 255;
	int zi = (int)point.z & 255;
	float xf = point.x - (int)point.x;
	float yf = point.y - (int)point.y;
	float zf = point.z - (int)point.z;
	float u = fade(xf);
	float v = fade(yf);
	float w = fade(zf);

	int aaa = p[p[p[xi] + yi] + zi];
	int aba = p[p[p[xi] + yi + 1] + zi];
	int aab = p[p[p[xi] + yi] + zi + 1];
	int abb = p[p[p[xi] + yi + 1] + zi + 1];
	int baa = p[p[p[xi + 1] + yi] + zi];
	int bba = p[p[p[xi + 1] + yi + 1] + zi];
	int bab = p[p[p[xi + 1] + yi] + zi];
	int bbb = p[p[p[xi + 1] + yi + 1] + zi + 1];

	float x1 = lerp(grad(aaa, xf, yf, zf), grad(baa, xf - 1, yf, zf), u);
	float x2 = lerp(grad(aba, xf, yf - 1, zf), grad(bba, xf - 1, yf - 1, zf), u);
	float y1 = lerp(x1, x2, v);
	x1 = lerp(grad(aab, xf, yf, zf - 1), grad(bab, xf - 1, yf, zf - 1), u);
	x2 = lerp(grad(abb, xf, yf - 1, zf - 1), grad(bbb, xf - 1, yf - 1, zf - 1), u);
	float y2 = lerp(x1, x2, v);
	return (lerp(y1, y2, w) + 1) / 2;
}

float Noise::rawNoise(Vector2 point) {
	Vector4 Pi = Vector4(floor(point.x), floor(point.y), floor(point.x), floor(point.y)) + Vector4(0, 0, 1, 1);
	Vector4 Pf = Vector4(fract(point.x), fract(point.y), fract(point.x), fract(point.y)) + Vector4(0, 0, 1, 1);
	Pi = mod(Pi, 289);
	Vector4 ix = Vector4(Pi.x, Pi.z, Pi.z, Pi.z);
	Vector4 iy = Vector4(Pi.y, Pi.y, Pi.w, Pi.w);
	Vector4 fx = Vector4(Pf.x, Pf.z, Pf.x, Pf.z);
	Vector4 fy = Vector4(Pf.y, Pf.y, Pf.w, Pf.w);
	Vector4 i = permute(permute(ix) + iy);
	Vector4 gx = (fract(i * 0.0243902439f) * 2) - Vector4(1, 1, 1, 1);
	Vector4 gy = Vector4(abs(gx.x), abs(gx.y), abs(gx.z), abs(gx.w)) - Vector4(0.5f, 0.5f, 0.5f, 0.5f);
	Vector4 tx = Vector4(floor(gx.x + 0.5), floor(gx.y + 0.5), floor(gx.z + 0.5), floor(gx.w + 0.5));
	gx -= tx;
	Vector2 g00 = Vector2(gx.x, gy.x);
	Vector2 g10 = Vector2(gx.y, gy.y);
	Vector2 g01 = Vector2(gx.z, gy.z);
	Vector2 g11 = Vector2(gx.w, gy.w);
	Vector4 norm = Vector4(1.79884291400159, 1.79884291400159, 1.79884291400159, 1.79884291400159) - (Vector4(g00.Dot(g00), g01.Dot(g01), g10.Dot(g10), g11.Dot(g11)) * 0.85373472095314);
	g00 *= norm.x;
	g01 *= norm.y;
	g10 *= norm.z;
	g11 *= norm.w;
	float n00 = g00.Dot(Vector2(fx.x, fy.x));
	float n10 = g10.Dot(Vector2(fx.y, fy.y));
	float n01 = g01.Dot(Vector2(fx.z, fy.z));
	float n11 = g11.Dot(Vector2(fx.w, fy.w));
	Vector2 fade_xy = Vector2(fade(Pf.x), fade(Pf.y));
	Vector2 n_x = lerp(Vector2(n00, n01), Vector2(n10, n11), fade_xy.x);
	float n_xy = lerp(n_x.x, n_x.y, fade_xy.y);
	return 2.3 * n_xy;
}

float Noise::noise(Vector3 point, int octaves, float persistence) {
	float total = 0;
	float frequency = 1;
	float amplitude = 1;
	float maxValue = 0;
	for (int i = 0; i < octaves; ++i) {
		total += rawNoise(Vector2(point.x * frequency, point.z * frequency)) * amplitude;
		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= 2;
	}

	return total / maxValue;
}