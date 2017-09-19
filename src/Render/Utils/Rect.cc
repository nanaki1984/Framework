#include <algorithm>
#include "Render/Utils/Rect.h"
#include "Math/Math.h"

namespace Framework {

Rect::Rect()
{ }

Rect::Rect(float x, float y, float width, float height)
: min(x, y),
  max(x + width, y + height)
{ }

Rect::Rect(const Math::Vector2 &_min, const Math::Vector2 &_max)
: min(_min),
  max(_max)
{ }

float
Rect::GetWidth() const
{
    return max.x - min.x;
}

float
Rect::GetHeight() const
{
    return max.y - min.y;
}

void
Rect::SetWidth(float width)
{
    max.x = min.x + width;
}

void
Rect::SetHeight(float height)
{
    max.y = min.y + height;
}

void
Rect::Reset()
{
    min.x = Math::PosInfinity;
    min.y = Math::PosInfinity;
    max.x = Math::NegInfinity;
    max.y = Math::NegInfinity;
}

void
Rect::Encapsulate(const Math::Vector2 &v)
{
    min.x = std::min(min.x, v.x);
    min.y = std::min(min.y, v.y);
    max.x = std::max(max.x, v.x);
    max.y = std::max(max.y, v.y);
}

void
Rect::Encapsulate(const Rect &r)
{
    min.x = std::min(min.x, r.min.x);
    min.y = std::min(min.y, r.min.y);
    max.x = std::max(max.x, r.max.x);
    max.y = std::max(max.y, r.max.y);
}

bool
Rect::Contains(const Rect &r) const
{
    return Math::Vector2::LessAll(min, r.min) &&
           Math::Vector2::GreaterEqualAll(max, r.max);
}

bool
Rect::Intersects(const Rect &r) const
{
    return !(Math::Vector2::LessAny(max, r.min) ||
             Math::Vector2::GreaterAny(min, r.max));
}

bool
Rect::operator ==(const Rect &r) const
{
    return min == r.min &&
           max == r.max;
}

bool
Rect::operator !=(const Rect &r) const
{
    return min != r.min ||
           max != r.max;
}

} // namespace Framework
