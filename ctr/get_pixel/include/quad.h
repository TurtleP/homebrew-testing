#pragma once

#include <3ds.h>

struct Viewport
{
    float x;
    float y;
    float w;
    float h;
};

struct Quad
{
    Viewport viewport;
    double width;
    double height;

    Tex3DS_SubTexture subTexture;

    Quad(const Viewport &viewport, float width, float height)
    {
        this->viewport = viewport;

        this->width = width;
        this->height = height;
    }

    const Tex3DS_SubTexture &CalculateViewport(const Viewport &viewport, C3D_Tex *texture)
    {
        this->subTexture.top = 1.0f - (viewport.y + 1.0f) / texture->height;
        this->subTexture.left = (1.0f + viewport.x) / texture->width;

        this->subTexture.right = (1.0f + viewport.x + viewport.w) / texture->width;
        this->subTexture.bottom = 1.0f - ((viewport.y + viewport.h + 1.0f) / texture->height);

        this->subTexture.width = viewport.w;
        this->subTexture.height = viewport.h;

        return this->subTexture;
    }
};

constexpr size_t LOVE_MIN_TEX = 8U;
constexpr size_t LOVE_MAX_TEX = 1024U;

/*
    ** Clamps 3DS textures between min
    ** and max texture size to prevent
    ** the GPU from locking up
    */
inline int NextPO2(size_t in)
{
    in--;
    in |= in >> 1;
    in |= in >> 2;
    in |= in >> 4;
    in |= in >> 8;
    in |= in >> 16;
    in++;

    return std::clamp(in, LOVE_MIN_TEX, LOVE_MAX_TEX);
}
