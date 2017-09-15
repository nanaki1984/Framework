#include "Core/Debug.h"
#include "Math/Math.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Framework {
	namespace Math {
    
const Vector3 Vector3::Zero  = Vector3(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::One   = Vector3(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::XAxis = Vector3(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::YAxis = Vector3(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::ZAxis = Vector3(0.0f, 0.0f, 1.0f);

const Vector3 Vector3::Corners[] = {
	Vector3(-1.0f, -1.0f, -1.0f),
	Vector3( 1.0f, -1.0f, -1.0f),
	Vector3(-1.0f,  1.0f, -1.0f),
	Vector3( 1.0f,  1.0f, -1.0f),
	Vector3(-1.0f, -1.0f,  1.0f),
	Vector3( 1.0f, -1.0f,  1.0f),
	Vector3(-1.0f,  1.0f,  1.0f),
	Vector3( 1.0f,  1.0f,  1.0f)
};

Vector3::Vector3()
{ }

Vector3::Vector3(float _x, float _y, float _z)
: x(_x),
  y(_y),
  z(_z)
{ }

Vector3::Vector3(const Vector2 &v2)
: x(v2.x),
  y(v2.y),
  z(0.0f)
{ }

Vector3::Vector3(const Vector4 &v4)
: x(v4.x),
  y(v4.y),
  z(v4.z)
{ }

Vector3::Vector3(const float *_xyz)
{
    x = *(_xyz++);
    y = *(_xyz++);
    z = *(_xyz);
}

Vector3&
Vector3::operator =(const Vector2 &v2)
{
    x = v2.x;
    y = v2.y;
    z = 0.0f;
    return (*this);
}

Vector3&
Vector3::operator =(const Vector4 &v4)
{
    x = v4.x;
    y = v4.y;
    z = v4.z;
    return (*this);
}

float&
Vector3::operator [](unsigned int i)
{
    assert(i >= 0 && i < 3);
    return xyz[i];
}

const float&
Vector3::operator [](unsigned int i) const
{
    assert(i >= 0 && i < 3);
    return xyz[i];
}

Vector3
Vector3::operator -() const
{
    return Vector3(-x, -y, -z);
}

Vector3
Vector3::operator *(float s) const
{
    return Vector3(x * s, y * s, z * s);
}

Vector3&
Vector3::operator *=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    return (*this);
}

Vector3
Vector3::operator /(float s) const
{
    return this->operator*(1.0f / s);
}

Vector3&
Vector3::operator /=(float s)
{
    return this->operator*=(1.0f / s);
}

Vector3
Vector3::operator +(const Vector3 &v) const
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3&
Vector3::operator +=(const Vector3 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return (*this);
}

Vector3
Vector3::operator -(const Vector3 &v) const
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3&
Vector3::operator -=(const Vector3 &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return (*this);
}

bool
Vector3::operator ==(const Vector3 &v) const
{
    return fabs(x - v.x) <= Math::Epsilon &&
           fabs(y - v.y) <= Math::Epsilon &&
           fabs(z - v.z) <= Math::Epsilon;
}

bool
Vector3::operator !=(const Vector3 &v) const
{
    return fabs(x - v.x) > Math::Epsilon ||
           fabs(y - v.y) > Math::Epsilon ||
           fabs(z - v.z) > Math::Epsilon;
}

float
Vector3::Dot(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3
Vector3::Cross(const Vector3 &v1, const Vector3 &v2)
{
    return Vector3((v1.y * v2.z) - (v1.z * v2.y),
                   (v1.z * v2.x) - (v1.x * v2.z),
                   (v1.x * v2.y) - (v1.y * v2.x));
}

float
Vector3::GetSqrMagnitude() const
{
    return Dot(*this, *this);
}

float
Vector3::GetMagnitude() const
{
    return sqrtf(Dot(*this, *this));
}

float
Vector3::Normalize()
{
    float m(this->GetMagnitude());
    
    if (m > Math::Epsilon)
        this->operator/=(m);
    
    return m;
}

Vector3
Vector3::GetNormalized() const
{
    float m(this->GetMagnitude());
    
    if (m > Math::Epsilon)
        return this->operator/(m);
    else
        return (*this);
}

bool
Vector3::LessEqualAll(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z;
}

bool
Vector3::LessAll(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z;
}

bool
Vector3::LessEqualAny(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x <= v2.x || v1.y <= v2.y || v1.z <= v2.z;
}

bool
Vector3::LessAny(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x < v2.x || v1.y < v2.y || v1.z < v2.z;
}

bool
Vector3::GreaterEqualAll(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z;
}

bool
Vector3::GreaterAll(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z;
}

bool
Vector3::GreaterEqualAny(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x >= v2.x || v1.y >= v2.y || v1.z >= v2.z;
}

bool
Vector3::GreaterAny(const Vector3 &v1, const Vector3 &v2)
{
    return v1.x > v2.x || v1.y > v2.y || v1.z > v2.z;
}

	} // namespace Math
} // namespace Framework
