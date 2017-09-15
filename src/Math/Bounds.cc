#include <algorithm>
#include "Math/Bounds.h"
#include "Math/Math.h"
#include "Math/Matrix.h"

namespace Framework {
	namespace Math {

Bounds::Bounds()
{ }

Bounds::Bounds(const Vector3 &center, const Vector3 &extents)
: min(center - extents),
  max(center + extents)
{ }

Vector3
Bounds::GetCenter() const
{
    return (max + min) * 0.5f;
}

Vector3
Bounds::GetExtents() const
{
    return (max - min) * 0.5f;
}

Vector3
Bounds::GetSize() const
{
    return max - min;
}

void
Bounds::GetVertices(Vector3 *v) const
{
    *(v++) = Vector3(min.x, min.y, min.z);
    *(v++) = Vector3(max.x, min.y, min.z);
    *(v++) = Vector3(max.x, max.y, min.z);
    *(v++) = Vector3(min.x, max.y, min.z);
    *(v++) = Vector3(min.x, min.y, max.z);
    *(v++) = Vector3(max.x, min.y, max.z);
    *(v++) = Vector3(max.x, max.y, max.z);
    *(v)   = Vector3(min.x, max.y, max.z);
}

void
Bounds::Reset()
{
    min.x = Math::PosInfinity;
    min.y = Math::PosInfinity;
    min.z = Math::PosInfinity;

    max.x = Math::NegInfinity;
    max.y = Math::NegInfinity;
    max.z = Math::NegInfinity;
}

void
Bounds::Encapsulate(const Math::Vector3 &v)
{
    min.x = std::min(min.x, v.x);
    min.y = std::min(min.y, v.y);
    min.z = std::min(min.z, v.z);

    max.x = std::max(max.x, v.x);
    max.y = std::max(max.y, v.y);
    max.z = std::max(max.z, v.z);
}

void
Bounds::Encapsulate(const Bounds &b)
{
    min.x = std::min(min.x, b.min.x);
    min.y = std::min(min.y, b.min.y);
    min.z = std::min(min.z, b.min.z);

    max.x = std::max(max.x, b.max.x);
    max.y = std::max(max.y, b.max.y);
    max.z = std::max(max.z, b.max.z);
}

void
Bounds::Transform(const Matrix &m)
{
    Vector3 v[8];
    this->GetVertices(v);
    this->Reset();
    for (int i = 0; i < 8; ++i)
        this->Encapsulate(m.FastMultiplyPoint(v[i]));
}

bool
Bounds::Contains(const Bounds &b) const
{
    return Math::Vector3::LessAll(min, b.min) &&
           Math::Vector3::GreaterEqualAll(max, b.max);
}

bool
Bounds::Intersects(const Bounds &b) const
{
    return !(Math::Vector3::LessAny(max, b.min) ||
             Math::Vector3::GreaterAny(min, b.max));
}

	} // namespace Math
} // namespace Framework
