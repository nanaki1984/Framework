#include <algorithm>
#include "Math/Math.h"
#include "Math/Vector3.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"

namespace Framework {
	namespace Math {

void
MatrixProjection(float fovy, float aspect, float znear, float zfar, Matrix &out)
{
	float scale = 1.0f / tanf(fovy * 0.5f);
	float q = 1.0f / (zfar - znear);

    out = Matrix::Zero;

    out(0, 0) = scale / aspect;
    out(1, 1) = scale;
    out(2, 2) = -(znear + zfar) * q;
    out(2, 3) = -1.0f;
	out(3, 2) = -2.0f * (znear * zfar) * q;
}

void
MatrixOrtho(float w, float h, float znear, float zfar, Matrix &out)
{
    out = Matrix::Zero;

    out(0, 0) = 2.0f / w;
    out(1, 1) = 2.0f / h;
    out(2, 2) = -2.0f / (zfar - znear);
	out(3, 2) = -(zfar + znear) / (zfar - znear);
    out(3, 3) = 1.0f;
}

void
MatrixLookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up, Matrix &out)
{
	Vector3 z = eye - target;
    z.Normalize();
	Vector3 x = Vector3::Cross(up, z);
    x.Normalize();
    Vector3 y = Vector3::Cross(z, x);

    out(0, 0) = x.x; out(0, 1) = x.y; out(0, 2) = x.z; out(0, 3) = 0.0f;
    out(1, 0) = y.x; out(1, 1) = y.y; out(1, 2) = y.z; out(1, 3) = 0.0f;
    out(2, 0) = z.x; out(2, 1) = z.y; out(2, 2) = z.z; out(2, 3) = 0.0f;

	out(3, 0) = eye.x;
	out(3, 1) = eye.y;
	out(3, 2) = eye.z;
    out(3, 3) = 1.0f;
}

void
QuaternionLookAt(const Vector3 &forward, const Vector3 &up, Quaternion &out)
{
	Matrix mat;
	MatrixLookAt(Vector3::Zero, forward, up, mat);
	out = Quaternion::FromMatrix(mat);
}

bool
IsPowerOf2(unsigned int n)
{
    return 0 == (n & (n - 1));
}

void
NextPowerOf2(unsigned int &n)
{
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    ++n;
}

unsigned int
HalfToFloat(unsigned short y)
{
    register int s = (y >> 15) & 0x00000001;
    register int e = (y >> 10) & 0x0000001f;
    register int m =  y        & 0x000003ff;
    
    if (e == 0)
    {
        if (m == 0) // Plus or minus zero
        {
            return s << 31;
        }
        else // Denormalized number -- renormalize it
        {
            while (!(m & 0x00000400))
            {
                m <<= 1;
                e -=  1;
            }
            
            e += 1;
            m &= ~0x00000400;
        }
    }
    else if (e == 31)
    {
        if (m == 0) // Inf
        {
            return (s << 31) | 0x7f800000;
        }
        else // NaN
        {
            return (s << 31) | 0x7f800000 | (m << 13);
        }
    }
    
    e = e + (127 - 15);
    m = m << 13;
    
    return (s << 31) | (e << 23) | m;
}

unsigned short
FloatToHalf(unsigned int i)
{
    register int s =  (i >> 16) & 0x00008000;
    register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
    register int m =   i        & 0x007fffff;
    
    if (e <= 0)
    {
        if (e < -10)
        {
            return 0;
        }
        m = (m | 0x00800000) >> (1 - e);
        
        return s | (m >> 13);
    }
    else if (e == 0xff - (127 - 15))
    {
        if (m == 0) // Inf
        {
            return s | 0x7c00;
        } 
        else    // NAN
        {
            m >>= 13;
            return s | 0x7c00 | m | (m == 0);
        }
    }
    else
    {
        if (e > 30) // Overflow
        {
            return s | 0x7c00;
        }
        
        return s | (e << 10) | (m >> 13);
    }
}

float
Clamp(float value, float min, float max)
{
    return std::min(std::max(value, min), max);
}

float
Clamp01(float value)
{
    return std::min(std::max(value, 0.0f), 1.0f);
}

float Lerp(float a, float b, float t)
{
    return a + (b - a) * Clamp01(t);
}

float AngleLerp(float a, float b, float t)
{
    float arc = b - a;
    if (arc > Pi)
        arc -= TwoPi;
    else if (arc < -Pi)
        arc += TwoPi;
    return a + arc * Clamp01(t);
}

uint32_t
MixHashCodes(uint32_t hash0, uint32_t hash1)
{
    return hash0 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2);
}

	} // namespace Math
} // namespace Framework
