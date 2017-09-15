#include "Core/Debug.h"
#include "Math/Math.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Framework {
	namespace Math {

const Vector2 Vector2::Zero  = Vector2(0.0f, 0.0f);
const Vector2 Vector2::One   = Vector2(1.0f, 1.0f);
const Vector2 Vector2::XAxis = Vector2(1.0f, 0.0f);
const Vector2 Vector2::YAxis = Vector2(0.0f, 1.0f);

Vector2::Vector2()
{ }

Vector2::Vector2(float _x, float _y)
: x(_x),
  y(_y)
{ }

Vector2::Vector2(const Vector3 &v3)
: x(v3.x),
  y(v3.y)
{ }

Vector2::Vector2(const Vector4 &v4)
: x(v4.x),
  y(v4.y)
{ }

Vector2::Vector2(const float *_xy)
{
    x = *(_xy++);
    y = *(_xy);
}

Vector2&
Vector2::operator =(const Vector3 &v3)
{
    x = v3.x;
    y = v3.y;
    return (*this);
}

Vector2&
Vector2::operator =(const Vector4 &v4)
{
    x = v4.x;
    y = v4.y;
    return (*this);
}

float&
Vector2::operator [](unsigned int i)
{
    assert(i >= 0 && i < 2);
    return xy[i];
}

const float&
Vector2::operator [](unsigned int i) const
{
    assert(i >= 0 && i < 2);
    return xy[i];
}

Vector2
Vector2::operator -() const
{
    return Vector2(-x, -y);
}

Vector2
Vector2::operator *(float s) const
{
    return Vector2(x * s, y * s);
}

Vector2&
Vector2::operator *=(float s)
{
    x *= s;
    y *= s;
    return (*this);
}

Vector2
Vector2::operator /(float s) const
{
    return this->operator*(1.0f / s);
}

Vector2&
Vector2::operator /=(float s)
{
    return this->operator*=(1.0f / s);
}

Vector2
Vector2::operator +(const Vector2 &v) const
{
    return Vector2(x + v.x, y + v.y);
}

Vector2&
Vector2::operator +=(const Vector2 &v)
{
    x += v.x;
    y += v.y;
    return (*this);
}

Vector2
Vector2::operator -(const Vector2 &v) const
{
    return Vector2(x - v.x, y - v.y);
}

Vector2&
Vector2::operator -=(const Vector2 &v)
{
    x -= v.x;
    y -= v.y;
    return (*this);
}

bool
Vector2::operator ==(const Vector2 &v) const
{
    return fabs(x - v.x) <= Math::Epsilon &&
           fabs(y - v.y) <= Math::Epsilon;
}

bool
Vector2::operator !=(const Vector2 &v) const
{
    return fabs(x - v.x) > Math::Epsilon ||
           fabs(y - v.y) > Math::Epsilon;
}

float
Vector2::Dot(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float
Vector2::Cross(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x * v2.y - v2.x * v1.y;
}

float
Vector2::GetSqrMagnitude() const
{
    return Dot(*this, *this);
}

float
Vector2::GetMagnitude() const
{
    return sqrtf(Dot(*this, *this));
}

float
Vector2::Normalize()
{
    float m(this->GetMagnitude());
    
    if (m > Math::Epsilon)
        this->operator/=(m);
    
    return m;
}

Vector2
Vector2::GetNormalized() const
{
    float m(this->GetMagnitude());
    
    if (m > Math::Epsilon)
        return this->operator/(m);
    else
        return (*this);
}

bool
Vector2::LessEqualAll(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x <= v2.x && v1.y <= v2.y;
}

bool
Vector2::LessAll(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x < v2.x && v1.y < v2.y;
}

bool
Vector2::LessEqualAny(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x <= v2.x || v1.y <= v2.y;
}

bool
Vector2::LessAny(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x < v2.x || v1.y < v2.y;
}

bool
Vector2::GreaterEqualAll(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x >= v2.x && v1.y >= v2.y;
}

bool
Vector2::GreaterAll(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x > v2.x && v1.y > v2.y;
}

bool
Vector2::GreaterEqualAny(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x >= v2.x || v1.y >= v2.y;
}

bool
Vector2::GreaterAny(const Vector2 &v1, const Vector2 &v2)
{
    return v1.x > v2.x || v1.y > v2.y;
}

	} // namespace Math
} // namespace Framework
