#pragma once

#include "Math/Vector3.h"

namespace Framework {
	namespace Math {

class Matrix;

class Bounds {
public:
    Vector3 min;
    Vector3 max;

    Bounds();
    Bounds(const Vector3 &center, const Vector3 &extents);

    Vector3 GetCenter() const;
    Vector3 GetExtents() const;
    Vector3 GetSize() const;

    void GetVertices(Vector3 *v) const;

    void Reset();
    void Encapsulate(const Vector3 &v);
    void Encapsulate(const Bounds &b);
    void Transform(const Matrix &m);

    bool Contains(const Bounds &b) const;
    bool Intersects(const Bounds &b) const;
};

	} // namespace Math
} // namespace Framework
