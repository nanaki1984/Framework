#pragma once

namespace Framework {

class Color;

	namespace Math {

class Vector2;
class Vector3;
class Quaternion;
class Plane;

class Vector4 {
public:
    static const Vector4 Zero;
    static const Vector4 One;
    
    union {
        struct {
            float x, y, z, w;
        };
        float xyzw[4];
    };
    
    Vector4();
    Vector4(float _x, float _y, float _z, float _w);
    Vector4(const Vector2 &v2);
    Vector4(const Vector3 &v3);
    explicit Vector4(const float *_xyzw);
    
    Vector4& operator =(const Vector2 &v2);
    Vector4& operator =(const Vector3 &v3);
    
    float& operator [](unsigned int i);
    const float& operator [](unsigned int i) const;
    
    Vector4 operator -() const;
    Vector4 operator *(float s) const;
    Vector4& operator *=(float s);
    Vector4 operator /(float s) const;
    Vector4& operator /=(float s);
    Vector4 operator +(const Vector4 &v) const;
    Vector4& operator +=(const Vector4 &v);
    Vector4 operator -(const Vector4 &v) const;
    Vector4& operator -=(const Vector4 &v);
    
    bool operator ==(const Vector4 &v) const;
    bool operator !=(const Vector4 &v) const;
    
    float GetSqrMagnitude() const;
    float GetMagnitude() const;
    
    float Normalize();
    Vector4 GetNormalized() const;
    
    static float Dot(const Vector4 &v1, const Vector4 &v2);

    static bool LessEqualAll(const Vector4 &v1, const Vector4 &v2);
    static bool LessAll(const Vector4 &v1, const Vector4 &v2);
    static bool LessEqualAny(const Vector4 &v1, const Vector4 &v2);
    static bool LessAny(const Vector4 &v1, const Vector4 &v2);
    static bool GreaterEqualAll(const Vector4 &v1, const Vector4 &v2);
    static bool GreaterAll(const Vector4 &v1, const Vector4 &v2);
    static bool GreaterEqualAny(const Vector4 &v1, const Vector4 &v2);
    static bool GreaterAny(const Vector4 &v1, const Vector4 &v2);

    const Quaternion& AsQuaternion() const;
    Quaternion& AsQuaternion();
    const Plane& AsPlane() const;
    Plane& AsPlane();
    const Color& AsColor() const;
    Color& AsColor();
};
    
	} // namespace Math
} // namespace Framework
