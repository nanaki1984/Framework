#include "Render/Utils/Color.h"
#include "Core/Debug.h"
#include "Math/Math.h"

namespace Framework {

const Color Color::Black   = Color(0.0f, 0.0f, 0.0f);
const Color Color::White   = Color(1.0f, 1.0f, 1.0f);
const Color Color::Red     = Color(1.0f, 0.0f, 0.0f);
const Color Color::Green   = Color(0.0f, 1.0f, 0.0f);
const Color Color::Blue    = Color(0.0f, 0.0f, 1.0f);
const Color Color::Yellow  = Color(1.0f, 1.0f, 0.0f);
const Color Color::Magenta = Color(1.0f, 0.0f, 1.0f);
const Color Color::Cyan    = Color(0.0f, 1.0f, 1.0f);

Color::Color()
{ }

Color::Color(float _r, float _g, float _b)
: r(_r),
  g(_g),
  b(_b),
  a(1.0f)
{ }

Color::Color(float _r, float _g, float _b, float _a)
: r(_r),
  g(_g),
  b(_b),
  a(_a)
{ }

Color::Color(const float *_rgba)
{
    r = *(_rgba++);
    g = *(_rgba++);
    b = *(_rgba++);
    a = *(_rgba);
}

Color::Color(uint32_t _argb)
{
    r = ((_argb & 0x00ff0000) >> 16) / 255.0f;
    g = ((_argb & 0x0000ff00) >>  8) / 255.0f;
    b = ((_argb & 0x000000ff) >>  0) / 255.0f;
    a = ((_argb & 0xff000000) >> 24) / 255.0f;
}

Color::operator uint32_t()
{
    return ((uint32_t)ceilf(a * 255.0f) << 24) |
           ((uint32_t)ceilf(r * 255.0f) << 16) |
           ((uint32_t)ceilf(g * 255.0f) <<  8) |
           ((uint32_t)ceilf(b * 255.0f) <<  0);
}

float&
Color::operator [](unsigned int i)
{
    assert(i < 4);
    return rgba[i];
}

const float&
Color::operator [](unsigned int i) const
{
    assert(i < 4);
    return rgba[i];
}

Color
Color::operator -() const
{
    return Color(-r, -g, -b, -a);
}

Color
Color::operator *(float s) const
{
    return Color(r * s, g * s, b * s, a * s);
}

Color&
Color::operator *=(float s)
{
    r *= s;
    g *= s;
    b *= s;
    a *= s;
    return (*this);
}

Color
Color::operator /(float s) const
{
    return this->operator*(1.0f / s);
}

Color&
Color::operator /=(float s)
{
    return this->operator*=(1.0f / s);
}

Color
Color::operator +(const Color &c) const
{
    return Color(r + c.r, g + c.g, b + c.b, a + c.a);
}

Color&
Color::operator +=(const Color &c)
{
    r += c.r;
    g += c.g;
    b += c.b;
    a += c.a;
    return (*this);
}

Color
Color::operator -(const Color &c) const
{
    return Color(r - c.r, g - c.g, b - c.b, a - c.a);
}

Color&
Color::operator -=(const Color &c)
{
    r -= c.r;
    g -= c.g;
    b -= c.b;
    a -= c.a;
    return (*this);
}

bool
Color::operator ==(const Color &c) const
{
    return fabs(r - c.r) <= Math::Epsilon &&
           fabs(g - c.g) <= Math::Epsilon &&
           fabs(b - c.b) <= Math::Epsilon &&
           fabs(a - c.a) <= Math::Epsilon;
}

bool
Color::operator !=(const Color &c) const
{
    return fabs(r - c.r) > Math::Epsilon ||
           fabs(g - c.g) > Math::Epsilon ||
           fabs(b - c.b) > Math::Epsilon ||
           fabs(a - c.a) > Math::Epsilon;
}

void
Color::PremultiplyAlpha()
{
    r *= a;
    g *= a;
    b *= a;
    a = 0.0f;
}

void
Color::Saturate()
{
    r = Math::Clamp01(r);
    g = Math::Clamp01(g);
    b = Math::Clamp01(b);
    a = Math::Clamp01(a);
}

Color
Color::GetSaturated() const
{
    return Color(Math::Clamp01(r), Math::Clamp01(g), Math::Clamp01(b), Math::Clamp01(a));
}

} // namespace Framework
