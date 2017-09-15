#pragma once

#include "Math/Vector3.h"

namespace Framework {
	namespace Math {

class Quaternion;

class AxisAngle {
public:
    static const AxisAngle Identity;

    Vector3 axis;
    float   angle;

    AxisAngle();
    AxisAngle(const Vector3 &_axis, float _angle);
    AxisAngle(float ax, float ay, float az, float _angle);
    AxisAngle(const Quaternion &q);
    explicit AxisAngle(const float *xyza);

    AxisAngle& operator =(const Quaternion &q);

    static AxisAngle RotateVector(const Vector3 &from, const Vector3 &to);
};

	} // namespace Math
} // namespace Framework
