#include "Core/Debug.h"
#include "Math/Math.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Plane.h"
#include "Render/Utils/Color.h"

namespace Framework {
	namespace Math {
    
const Vector4 Vector4::Zero  = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::One   = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

Vector4::Vector4()
{ }

Vector4::Vector4(float _x, float _y, float _z, float _w)
: x(_x),
  y(_y),
  z(_z),
  w(_w)
{ }

Vector4::Vector4(const Vector2 &v2)
: x(v2.x),
  y(v2.y),
  z(0.0f),
  w(1.0f)
{ }

Vector4::Vector4(const Vector3 &v3)
: x(v3.x),
  y(v3.y),
  z(v3.z),
  w(1.0f)
{ }

Vector4::Vector4(const float *_xyzw)
{
    x = *(_xyzw++);
    y = *(_xyzw++);
    z = *(_xyzw++);
    w = *(_xyzw);
}

Vector4&
Vector4::operator =(const Vector2 &v2)
{
    x = v2.x;
    y = v2.y;
    z = 0.0f;
    w = 1.0f;
    return (*this);
}

Vector4&
Vector4::operator =(const Vector3 &v3)
{
    x = v3.x;
    y = v3.y;
    z = v3.z;
    w = 1.0f;
    return (*this);
}

float&
Vector4::operator [](unsigned int i)
{
    assert(i < 4);
    return xyzw[i];
}

const float&
Vector4::operator [](unsigned int i) const
{
    assert(i < 4);
    return xyzw[i];
}

Vector4
Vector4::operator -() const
{
    return Vector4(-x, -y, -z, -w);
}

Vector4
Vector4::operator *(float s) const
{
    return Vector4(x * s, y * s, z * s, w * s);
}

Vector4&
Vector4::operator *=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return (*this);
}

Vector4
Vector4::operator /(float s) const
{
    return this->operator*(1.0f / s);
}

Vector4&
Vector4::operator /=(float s)
{
    return this->operator*=(1.0f / s);
}

Vector4
Vector4::operator +(const Vector4 &v) const
{
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4&
Vector4::operator +=(const Vector4 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return (*this);
}

Vector4
Vector4::operator -(const Vector4 &v) const
{
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4&
Vector4::operator -=(const Vector4 &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return (*this);
}

bool
Vector4::operator ==(const Vector4 &v) const
{
    return fabs(x - v.x) <= Math::Epsilon &&
           fabs(y - v.y) <= Math::Epsilon &&
           fabs(z - v.z) <= Math::Epsilon &&
           fabs(w - v.w) <= Math::Epsilon;
}

bool
Vector4::operator !=(const Vector4 &v) const
{
    return fabs(x - v.x) > Math::Epsilon ||
           fabs(y - v.y) > Math::Epsilon ||
           fabs(z - v.z) > Math::Epsilon ||
           fabs(w - v.w) > Math::Epsilon;
}

float
Vector4::Dot(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

float
Vector4::GetSqrMagnitude() const
{
    return Dot(*this, *this);
}

float
Vector4::GetMagnitude() const
{
    return sqrtf(Dot(*this, *this));
}

float
Vector4::Normalize()
{
    float m(this->GetMagnitude());
    
    if (m > Math::Epsilon)
        this->operator/=(m);
    
    return m;
}

Vector4
Vector4::GetNormalized() const
{
    float m(this->GetMagnitude());
    
    if (m > Math::Epsilon)
        return this->operator/(m);
    else
        return (*this);
}

bool
Vector4::LessEqualAll(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z && v1.w <= v2.w;
}

bool
Vector4::LessAll(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z && v1.w < v2.w;
}

bool
Vector4::LessEqualAny(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x <= v2.x || v1.y <= v2.y || v1.z <= v2.z || v1.w <= v2.w;
}

bool
Vector4::LessAny(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x < v2.x || v1.y < v2.y || v1.z < v2.z || v1.w < v2.w;
}

bool
Vector4::GreaterEqualAll(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z && v1.w >= v2.w;
}

bool
Vector4::GreaterAll(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z && v1.w > v2.w;
}

bool
Vector4::GreaterEqualAny(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x >= v2.x || v1.y >= v2.y || v1.z >= v2.z || v1.w >= v2.w;
}

bool
Vector4::GreaterAny(const Vector4 &v1, const Vector4 &v2)
{
    return v1.x > v2.x || v1.y > v2.y || v1.z > v2.z || v1.w > v2.w;
}

const Quaternion&
Vector4::AsQuaternion() const
{
    return *reinterpret_cast<const Quaternion*>(this);
}

Quaternion&
Vector4::AsQuaternion()
{
    return *reinterpret_cast<Quaternion*>(this);
}

const Plane&
Vector4::AsPlane() const
{
    return *reinterpret_cast<const Plane*>(this);
}

Plane&
Vector4::AsPlane()
{
    return *reinterpret_cast<Plane*>(this);
}

const Color&
Vector4::AsColor() const
{
    return *reinterpret_cast<const Color*>(this);
}

Color&
Vector4::AsColor()
{
    return *reinterpret_cast<Color*>(this);
}

	} // namespace Math
} // namespace Framework
