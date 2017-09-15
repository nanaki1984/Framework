#pragma once

namespace Framework {
	namespace Math {

class Vector3;
class Vector4;

class Plane {
public:
    union {
        struct {
            float a, b, c, d;
        };
        float abcd[4];
    };

    Plane();
    Plane(const Vector3 &n, float _d);
    Plane(float _a, float _b, float _c, float _d);
    explicit Plane(const float *_abcd);

    float Normalize();
    Plane GetNormalized() const;

    const Vector3& GetNormal() const;
    float GetPointDistance(const Vector3 &p) const;
    Vector3 NearestPoint(const Vector3 &p) const;
    Vector3 ReflectPoint(const Vector3 &p) const;
    Vector3 ReflectVector(const Vector3 &v) const;

    const Vector4& AsVector4() const;
    Vector4& AsVector4();

    static Plane FromTriangle(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
};

	} // namespace Math
} // namespace Framework
