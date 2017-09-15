#include "Core/Debug.h"
#include "Math/Math.h"
#include "Math/Quaternion.h"
#include "Math/AxisAngle.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"

namespace Framework {
	namespace Math {

const Quaternion Quaternion::Identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

Quaternion::Quaternion()
{ }

Quaternion::Quaternion(float _i, float _j, float _k, float _w)
: i(_i),
  j(_j),
  k(_k),
  w(_w)
{ }

Quaternion::Quaternion(const AxisAngle &aa)
{
    float ha = aa.angle * 0.5f * Math::Deg2Rad,
          s = sinf(ha),
          c = cosf(ha);

    i = aa.axis.x * s;
    j = aa.axis.y * s;
    k = aa.axis.z * s;
    w = c;
}

Quaternion::Quaternion(const float *_ijkw)
{
    i = *(_ijkw++);
    j = *(_ijkw++);
    k = *(_ijkw++);
    w = *(_ijkw);
}

Quaternion&
Quaternion::operator =(const AxisAngle &aa)
{
    float ha = aa.angle * 0.5f * Math::Deg2Rad,
    s = sinf(ha),
    c = cosf(ha);
    
    i = aa.axis.x * s;
    j = aa.axis.y * s;
    k = aa.axis.z * s;
    w = c;

    return (*this);
}

float&
Quaternion::operator [](unsigned int i)
{
    assert(i < 4);
    return ijkw[i];
}

const float&
Quaternion::operator [](unsigned int i) const
{
    assert(i < 4);
    return ijkw[i];
}

Quaternion
Quaternion::operator -() const
{
    return Quaternion(-i, -j, -k, -w);
}

Quaternion
Quaternion::operator *(float s) const
{
    return Quaternion(i * s, j * s, k * s, w * s);
}

Quaternion&
Quaternion::operator *=(float s)
{
    i *= s;
    j *= s;
    k *= s;
    w *= s;
    return (*this);
}

Quaternion
Quaternion::operator /(float s) const
{
    return this->operator*(1.0f / s);
}

Quaternion&
Quaternion::operator /=(float s)
{
    return this->operator*=(1.0f / s);
}

Quaternion
Quaternion::operator +(const Quaternion &q) const
{
    return Quaternion(i + q.i, j + q.j, k + q.k, w + q.w);
}

Quaternion&
Quaternion::operator +=(const Quaternion &q)
{
    i += q.i;
    j += q.j;
    k += q.k;
    w += q.w;
    return (*this);
}

Quaternion
Quaternion::operator -(const Quaternion &q) const
{
    return Quaternion(i - q.i, j - q.j, k - q.k, w - q.w);
}

Quaternion&
Quaternion::operator -=(const Quaternion &q)
{
    i -= q.i;
    j -= q.j;
    k -= q.k;
    w -= q.w;
    return (*this);
}

Quaternion
Quaternion::operator *(const Quaternion &q) const
{
    return Quaternion(
        q.i * w + i * q.w + (j * q.k) - (q.j * k),
        q.j * w + j * q.w + (k * q.i) - (q.k * i),
        q.k * w + k * q.w + (i * q.j) - (q.i * j),
        w * q.w - (i * q.i + j * q.j + k * q.k));
}

Quaternion&
Quaternion::operator *=(const Quaternion &q)
{
    (*this) = this->operator*(q);
    return (*this);
}

Quaternion
Quaternion::operator *(const Vector3 &v) const
{
    return Quaternion(
        v.x * w + (j * v.z) - (v.y * k),
        v.y * w + (k * v.x) - (v.z * i),
        v.z * w + (i * v.y) - (v.x * j),
        -(i * v.x + j * v.y + k * v.z));
}

Quaternion&
Quaternion::operator *=(const Vector3 &v)
{
    (*this) = this->operator*(v);
    return (*this);
}

Quaternion
Quaternion::GetConjugate() const
{
    return Quaternion(-i, -j, -k, w);
}

Quaternion
Quaternion::GetInverse() const
{
    float OoNorm = this->GetSqrMagnitude();

    if (OoNorm > Math::Epsilon)
        OoNorm = 1.0f / OoNorm;
    else
        OoNorm = 1.0f;

    return Quaternion(-i * OoNorm, -j  * OoNorm, -k * OoNorm, w * OoNorm);
}

Vector3
Quaternion::Rotate(const Vector3 &v) const
{
    Quaternion q = ((*this) * v * this->GetConjugate());
    return Vector3(q.i, q.j, q.k);
}

Vector3
Quaternion::InverseRotate(const Vector3 &v) const
{
    Quaternion q = this->GetConjugate() * v * (*this);
    return Vector3(q.i, q.j, q.k);
}

Vector3
Quaternion::GetAxis() const
{
    float OoSin = sqrtf(i * i + j * j + k * k);

    if (OoSin > Math::Epsilon) {
        OoSin = 1.f / OoSin;
        return Vector3(i * OoSin, j * OoSin, k * OoSin);
    }

    return Vector3::Zero;
}

float
Quaternion::GetAngle() const
{
    float mag = this->GetMagnitude();

    if (mag > Math::Epsilon)
        return 2.0f * acosf(w / mag) * Math::Rad2Deg;

    return 0.0f;
}

float
Quaternion::GetSqrMagnitude() const
{
    return Vector4::Dot(this->AsVector4(), this->AsVector4());
}

float
Quaternion::GetMagnitude() const
{
    return this->AsVector4().GetMagnitude();
}

float
Quaternion::Normalize()
{
    return this->AsVector4().Normalize();
}

Quaternion
Quaternion::GetNormalized() const
{
    return this->AsVector4().GetNormalized().AsQuaternion();
}

const Vector4&
Quaternion::AsVector4() const
{
    return *reinterpret_cast<const Vector4*>(this);
}

Vector4&
Quaternion::AsVector4()
{
    return *reinterpret_cast<Vector4*>(this);
}

Quaternion
Quaternion::FromMatrix(const Matrix &m)
{
    float diagonal, scale, oos, i, j, k, w;

    diagonal = m(0,0) + m(1,1) + m(2,2) + 1.0f;

    if (diagonal > Math::Epsilon) {
        scale = sqrtf(diagonal) * 2.0f;
        oos   = 1.0f / scale;

        i = (m(1,2) - m(2,1)) * oos;
        j = (m(2,0) - m(0,2)) * oos;
        k = (m(0,1) - m(1,0)) * oos;
        w = 0.25f * scale;
    } else {
        if (m(0,0) > m(1,1) && m(0,0) > m(2,2)) {
            scale = sqrtf(1.0f + m(0,0) - m(1,1) - m(2,2)) * 2.0f;
            oos   = 1.0f / scale;
            
            i = -0.25f * scale;
            j = -(m(1,0) + m(0,1)) * oos;
            k = -(m(0,2) + m(2,0)) * oos;
            w =  (m(1,2) - m(2,1)) * oos;
        } else if ( m(1,1) > m(2,2) ) {
            scale = sqrtf(1.0f + m(1,1) - m(0,0) - m(2,2)) * 2.0f;
            oos   = 1.0f / scale;
            
            i = -(m(1,0) + m(0,1)) * oos;
            j = -0.25f * scale;
            k = -(m(2,1) + m(1,2)) * oos;
            w =  (m(2,0) - m(0,2)) * oos;
        } else {
            scale = sqrtf(1.0f + m(2,2) - m(0,0) - m(1,1)) * 2.0f;
            oos   = 1.0f / scale;
            
            i = -(m(0,2) + m(2,0)) * oos;
            j = -(m(2,1) + m(1,2)) * oos;
            k = -0.25f * scale;
            w =  (m(0,1) - m(1,0)) * oos;
        }
    }
    
    float OoNorm = i * i + j * j + k * k + w * w;
    if (OoNorm > Math::Epsilon)
    {
        OoNorm = 1.0f / sqrtf(OoNorm);
        
        return Quaternion(i * OoNorm, j * OoNorm, k * OoNorm, w * OoNorm);
    }
    
    return Quaternion(i, j, k, w);
}

Quaternion
Quaternion::Slerp(const Quaternion &q1, const Quaternion &q2, float t)
{
    float cosom(Vector4::Dot(q1.AsVector4(), q2.AsVector4())),
          sinom, omega, s0, s1,
          to[4];
    
    if (cosom < 0.0f) {
        cosom = -cosom;
        to[0] = -q2.i;
        to[1] = -q2.j;
        to[2] = -q2.k;
        to[3] = -q2.w;
    } else {
        to[0] = q2.i;
        to[1] = q2.j;
        to[2] = q2.k;
        to[3] = q2.w;
    }
    
    t = Clamp01(t);
    float s = 1.0f - t;

    if ((1.0f - cosom) > Math::Epsilon) {
        omega = acosf(cosom);
        sinom = sinf(omega);

        s0 = sinf(s * omega) / sinom;
        s1 = sinf(t * omega) / sinom;
    } else {
        s0 = s;
        s1 = t;
    }

    return Quaternion((s0 * q1.i) + (s1 * to[0]),
                      (s0 * q1.j) + (s1 * to[1]),
                      (s0 * q1.k) + (s1 * to[2]),
                      (s0 * q1.w) + (s1 * to[3]));
}

	} // namespace Math
} // namespace Framework
