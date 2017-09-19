#pragma once

#include "Math/Vector2.h"

namespace Framework {

class Rect {
public:
    Math::Vector2 min;
    Math::Vector2 max;

    Rect();
    Rect(float x, float y, float width, float height);
    Rect(const Math::Vector2 &_min, const Math::Vector2 &_max);

    float GetWidth() const;
    float GetHeight() const;

    void SetWidth(float width);
    void SetHeight(float height);

    void Reset();
    void Encapsulate(const Math::Vector2 &v);
    void Encapsulate(const Rect &r);

    bool Contains(const Rect &r) const;
    bool Intersects(const Rect &r) const;

    bool operator ==(const Rect &r) const;
    bool operator !=(const Rect &r) const;
};

} // namespace Framework
