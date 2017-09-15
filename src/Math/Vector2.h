#pragma once

namespace Framework {
	namespace Math {

class Vector3;
class Vector4;

class Vector2 {
public:
    static const Vector2 Zero;
    static const Vector2 One;
    static const Vector2 XAxis;
    static const Vector2 YAxis;

    union {
        struct {
            float x, y;
        };
        float xy[2];
    };

    Vector2();
    Vector2(float _x, float _y);
    Vector2(const Vector3 &v3);
    Vector2(const Vector4 &v4);
    explicit Vector2(const float *_xy);

    Vector2& operator =(const Vector3 &v3);
    Vector2& operator =(const Vector4 &v4);

    float& operator [](unsigned int i);
    const float& operator [](unsigned int i) const;
    
    Vector2 operator -() const;
    Vector2 operator *(float s) const;
    Vector2& operator *=(float s);
    Vector2 operator /(float s) const;
    Vector2& operator /=(float s);
    Vector2 operator +(const Vector2 &v) const;
    Vector2& operator +=(const Vector2 &v);
    Vector2 operator -(const Vector2 &v) const;
    Vector2& operator -=(const Vector2 &v);

    bool operator ==(const Vector2 &v) const;
    bool operator !=(const Vector2 &v) const;
    
    float GetSqrMagnitude() const;
    float GetMagnitude() const;
    
    float Normalize();
    Vector2 GetNormalized() const;

    static float Dot(const Vector2 &v1, const Vector2 &v2);
    static float Cross(const Vector2 &v1, const Vector2 &v2);

    static bool LessEqualAll(const Vector2 &v1, const Vector2 &v2);
    static bool LessAll(const Vector2 &v1, const Vector2 &v2);
    static bool LessEqualAny(const Vector2 &v1, const Vector2 &v2);
    static bool LessAny(const Vector2 &v1, const Vector2 &v2);
    static bool GreaterEqualAll(const Vector2 &v1, const Vector2 &v2);
    static bool GreaterAll(const Vector2 &v1, const Vector2 &v2);
    static bool GreaterEqualAny(const Vector2 &v1, const Vector2 &v2);
    static bool GreaterAny(const Vector2 &v1, const Vector2 &v2);
};
 
	} // namespace Math
} // namespace Framework
