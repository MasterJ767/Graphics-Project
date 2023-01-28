#pragma once

#include <cmath>
#include "../nclgl/OGLRenderer.h"

class Noise {
public:
	Noise();
	~Noise();

	float rawNoise(Vector3 point);
	float rawNoise(Vector2 point);
	float noise(Vector3 point, int octaves, float persistence);
protected:
	float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	float lerp(float a, float b, float t) { return (b - a) * t + a; }
	float grad(int hash, float x, float y, float z);
	inline float mod(float num, int div);
	inline float fract(float num);
	inline float permute(float num);
	Vector2 lerp(Vector2 a, Vector2 b, float t) { return Vector2((b.x - a.x) * t + a.x, (b.y - a.y) * t + a.y); }
	Vector4 mod(Vector4 vec4, int div);
	Vector4 fract(Vector4 vec4);
	Vector4 permute(Vector4 vec4);

	int* p;
};