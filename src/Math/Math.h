#pragma once

#include <cmath>
#include <limits>
#include <cstdint>

namespace Framework {
	namespace Math {

const float Epsilon       = 1.0e-6f;
const float InvEpsilon    = 1.0f / Epsilon;
const float SqrEpsilon    = Epsilon * Epsilon;
const float InvSqrEpsilon = 1.0f / SqrEpsilon;
const float Pi            = 3.14159265358979323846f;
const float HalfPi        = Pi * 0.5f;
const float TwoPi         = Pi * 2.0f;
const float PosInfinity   =  std::numeric_limits<float>::infinity();
const float NegInfinity   = -std::numeric_limits<float>::infinity();
const float Deg2Rad       = Pi / 180.0f;
const float Rad2Deg       = 180.0f / Pi;

class Vector3;
class Matrix;
class Quaternion;

void MatrixProjection(float fovy, float aspect, float znear, float zfar, Matrix &out);
void MatrixOrtho(float w, float h, float znear, float zfar, Matrix &out);
void MatrixLookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up, Matrix &out);

void QuaternionLookAt(const Vector3 &forward, const Vector3 &up, Quaternion &out);

bool IsPowerOf2(unsigned int n);
void NextPowerOf2(unsigned int &n);

unsigned int HalfToFloat(unsigned short y);
unsigned short FloatToHalf(unsigned int i);

float Clamp(float value, float min, float max);
float Clamp01(float value);
float Lerp(float a, float b, float t);
float AngleLerp(float a, float b, float t);

uint32_t MixHashCodes(uint32_t hash0, uint32_t hash1);

	} // namespace Math
} // namespace Framework
