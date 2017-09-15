#pragma once

namespace Framework {
	namespace Math {

class AxisAngle;
class Vector3;
class Vector4;
class Matrix;

class Quaternion {
public:
    static const Quaternion Identity;

    union {
        struct {
            float i, j, k, w;
        };
        float ijkw[4];
    };

    Quaternion();
    Quaternion(float _i, float _j, float _k, float _w);
    Quaternion(const AxisAngle &aa);
    explicit Quaternion(const float *_ijkw);

    Quaternion& operator =(const AxisAngle &aa);

    float& operator [](unsigned int i);
    const float& operator [](unsigned int i) const;
    
    Quaternion operator -() const;
    Quaternion operator *(float s) const;
    Quaternion& operator *=(float s);
    Quaternion operator /(float s) const;
    Quaternion& operator /=(float s);
    Quaternion operator +(const Quaternion &q) const;
    Quaternion& operator +=(const Quaternion &q);
    Quaternion operator -(const Quaternion &q) const;
    Quaternion& operator -=(const Quaternion &q);

    Quaternion operator *(const Quaternion &q) const;
    Quaternion& operator *=(const Quaternion &q);
    Quaternion operator *(const Vector3 &v) const;
    Quaternion& operator *=(const Vector3 &v);

    Quaternion GetConjugate() const;
    Quaternion GetInverse() const;

    Vector3 Rotate(const Vector3 &v) const;
    Vector3 InverseRotate(const Vector3 &v) const;

    Vector3 GetAxis() const;
    float GetAngle() const;
    float GetSqrMagnitude() const;
    float GetMagnitude() const;

    float Normalize();
    Quaternion GetNormalized() const;

    const Vector4& AsVector4() const;
    Vector4& AsVector4();

    static Quaternion FromMatrix(const Matrix &m);
    static Quaternion Slerp(const Quaternion &q1, const Quaternion &q2, float t);
};

	} // namespace Math
} // namespace Framework
