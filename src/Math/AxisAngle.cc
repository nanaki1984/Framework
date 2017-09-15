#include "Math/Math.h"
#include "Math/AxisAngle.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace Framework {
	namespace Math {

AxisAngle::AxisAngle()
{ }

AxisAngle::AxisAngle(const Vector3 &_axis, float _angle)
: axis(_axis),
  angle(_angle)
{ }

AxisAngle::AxisAngle(float ax, float ay, float az, float _angle)
: axis(ax, ay, az),
  angle(_angle)
{ }

AxisAngle::AxisAngle(const Quaternion &q)
: axis(q.GetAxis()),
  angle(q.GetAngle())
{ }

AxisAngle::AxisAngle(const float *xyza)
: axis(xyza),
  angle(xyza[3])
{ }

AxisAngle&
AxisAngle::operator =(const Quaternion &q)
{
    axis = q.GetAxis();
    angle = q.GetAngle();
    return (*this);
}

AxisAngle
AxisAngle::RotateVector(const Vector3 &from, const Vector3 &to)
{
    Vector3 fromN = from.GetNormalized(),
            toN   = to.GetNormalized(),
            axis  = Vector3::Cross(fromN, toN);

    float s = axis.Normalize(),
          c = Vector3::Dot(fromN, toN);

    float angle = 0.0f;
    if (fabs(c) > Math::Epsilon)
        angle = atan(s / c) * Math::Rad2Deg;
    else
        angle = 180.0f * (s >= 0.0f ? 1.0f : -1.0f);

    return AxisAngle(axis, angle);
}

	} // namespace Math
} // namespace Framework
