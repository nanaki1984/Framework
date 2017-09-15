#include "Math/Plane.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Framework {
	namespace Math {

Plane::Plane()
{ }

Plane::Plane(const Vector3 &n, float _d)
: a(n.x),
  b(n.y),
  c(n.z),
  d(_d)
{ }

Plane::Plane(float _a, float _b, float _c, float _d)
: a(_a),
  b(_b),
  c(_c),
  d(_d)
{ }

Plane::Plane(const float *_abcd)
{
    a = *(_abcd++);
    b = *(_abcd++);
    c = *(_abcd++);
    d = *(_abcd);
}

float
Plane::Normalize()
{
    return this->AsVector4().Normalize();
}

Plane
Plane::GetNormalized() const
{
    return this->AsVector4().GetNormalized().AsPlane();
}

const Vector3&
Plane::GetNormal() const
{
    return *reinterpret_cast<const Vector3*>(this);
}

float
Plane::GetPointDistance(const Vector3 &p) const
{
    return p.x * a + p.y * b + p.z * c + d;
}

Vector3
Plane::NearestPoint(const Vector3 &p) const
{
    return p - (this->GetNormal() * this->GetPointDistance(p));
}

Vector3
Plane::ReflectPoint(const Vector3 &p) const
{
    return p - (this->GetNormal() * 2.0f * this->GetPointDistance(p));
}

Vector3
Plane::ReflectVector(const Vector3 &v) const
{
    return v - (this->GetNormal() * 2.0f * Vector3::Dot(this->GetNormal(), v));
}

const Vector4&
Plane::AsVector4() const
{
    return *reinterpret_cast<const Vector4*>(this);
}

Vector4&
Plane::AsVector4()
{
    return *reinterpret_cast<Vector4*>(this);
}

Plane
Plane::FromTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
    Vector3 a(v2 - v1), b(v3 - v1), n(Vector3::Cross(a, b));
    return Plane(n, -Vector3::Dot(n, v1));
}

	} // namespace Math
} // namespace Framework
