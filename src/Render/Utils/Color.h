#pragma once

#include <cstdint>

namespace Framework {

class Color {
public:
    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Magenta;
    static const Color Cyan;

    union {
        struct {
            float r, g, b, a;
        };
        float rgba[4];
    };

    Color();
    Color(float _r, float _g, float _b);
    Color(float _r, float _g, float _b, float _a);
    explicit Color(const float *_rgba);
    explicit Color(uint32_t _argb);

    operator uint32_t();

    float& operator [](unsigned int i);
    const float& operator [](unsigned int i) const;

    Color operator -() const;
    Color operator *(float s) const;
    Color& operator *=(float s);
    Color operator /(float s) const;
    Color& operator /=(float s);
    Color operator +(const Color &c) const;
    Color& operator +=(const Color &c);
    Color operator -(const Color &c) const;
    Color& operator -=(const Color &c);

    bool operator ==(const Color &c) const;
    bool operator !=(const Color &c) const;

    void PremultiplyAlpha();
    void Saturate();
    Color GetSaturated() const;
};

} // namespace Framework
