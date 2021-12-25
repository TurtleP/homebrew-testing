#pragma once

#include <citro2d.h>
#include <stdio.h>

struct Color
{
    float r;
    float g;
    float b;
    float a;

    Color() : r(0), g(0), b(0), a(1)
    {}

    Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a)
    {}

    bool operator==(const Color& other)
    {
        return (this->r == other.r) && (this->b == other.b) && (this->g == other.g) &&
               (this->a == other.a);
    }

    const char* colorName()
    {
        if (*this == Color(1.0f, 0.0f, 0.0f, 1.0f))
            return "R";
        else if (*this == Color(0.0f, 1.0f, 0.0f, 1.0f))
            return "G";
        else if (*this == Color(0.0f, 0.0f, 1.0f, 1.0f))
            return "B";
        else if (*this == Color(1.0f, 1.0f, 1.0f, 1.0f))
            return "W";
        else if (*this == Color(0.0f, 0.0f, 0.0f, 1.0f))
            return "K";
        else if (*this == Color(0.0f, 0.0f, 0.0f, 0.0f))
            return "T";
        else
            return "U";
    }
};

float colorFromBytes(unsigned color)
{
    return color / 255.0f;
}

const unsigned setPixelColors(float r, float g, float b, float a)
{
    return C2D_Color32f(r, g, b, a);
}

const Color getPixelColors(unsigned colors)
{
    printf("Colors: %u\n", colors);

    Color color {};

    color.r = colorFromBytes((colors & 0xFF000000) >> 0x18);
    color.g = colorFromBytes((colors & 0x00FF0000) >> 0x10);
    color.b = colorFromBytes((colors & 0x0000FF00) >> 0x08);
    color.a = colorFromBytes((colors & 0x000000FF) >> 0x00);

    return color;
}

// clang-format off
/// \brief Convert 3DS texture coordinates to pixel index
/// \param width_ Texture width (must be multiple of 8)
/// \param x_ X coordinate
/// \param y_ Y coordinate
unsigned coordToIndex (unsigned const width_, unsigned const x_, unsigned const y_)
{
    static unsigned char const table[] =
    {
         0,  1,  4,  5, 16, 17, 20, 21,
         2,  3,  6,  7, 18, 19, 22, 23,
         8,  9, 12, 13, 24, 25, 28, 29,
        10, 11, 14, 15, 26, 27, 30, 31,
        32, 33, 36, 37, 48, 49, 52, 53,
        34, 35, 38, 39, 50, 51, 54, 55,
        40, 41, 44, 45, 56, 57, 60, 61,
        42, 43, 46, 47, 58, 59, 62, 63,
    };

    unsigned const tileX = x_ / 8;
    unsigned const tileY = y_ / 8;
    unsigned const subX  = x_ % 8;
    unsigned const subY  = y_ % 8;

    return ((width_ / 8) * tileY + tileX) * 64 + table[subY * 8 + subX];
}
// clang-format on
