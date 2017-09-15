#pragma once

namespace Framework {
	namespace Math {
    
class Vector2;
class Vector4;

class Vector3 {
public:
    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 XAxis;
    static const Vector3 YAxis;
    static const Vector3 ZAxis;

	static const Vector3 Corners[];
    
    union {
        struct {
            float x, y, z;
        };
        float xyz[3];
    };
    
    Vector3();
    Vector3(float _x, float _y, float _z);
    Vector3(const Vector2 &v2);
    Vector3(const Vector4 &v4);
    explicit Vector3(const float *_xyz);

    Vector3& operator =(const Vector2 &v2);
    Vector3& operator =(const Vector4 &v4);

    float& operator [](unsigned int i);
    const float& operator [](unsigned int i) const;

    Vector3 operator -() const;
    Vector3 operator *(float s) const;
    Vector3& operator *=(float s);
    Vector3 operator /(float s) const;
    Vector3& operator /=(float s);
    Vector3 operator +(const Vector3 &v) const;
    Vector3& operator +=(const Vector3 &v);
    Vector3 operator -(const Vector3 &v) const;
    Vector3& operator -=(const Vector3 &v);

    bool operator ==(const Vector3 &p) const;
    bool operator !=(const Vector3 &p) const;

    float GetSqrMagnitude() const;
    float GetMagnitude() const;

    float Normalize();
    Vector3 GetNormalized() const;
    
    static float Dot(const Vector3 &v1, const Vector3 &v2);
    static Vector3 Cross(const Vector3 &v1, const Vector3 &v2);

    static bool LessEqualAll(const Vector3 &v1, const Vector3 &v2);
    static bool LessAll(const Vector3 &v1, const Vector3 &v2);
    static bool LessEqualAny(const Vector3 &v1, const Vector3 &v2);
    static bool LessAny(const Vector3 &v1, const Vector3 &v2);
    static bool GreaterEqualAll(const Vector3 &v1, const Vector3 &v2);
    static bool GreaterAll(const Vector3 &v1, const Vector3 &v2);
    static bool GreaterEqualAny(const Vector3 &v1, const Vector3 &v2);
    static bool GreaterAny(const Vector3 &v1, const Vector3 &v2);
};
    
	} // namespace Math
} // namespace Framework
