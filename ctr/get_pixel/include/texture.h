#pragma once

#include "color.h"
#include "quad.h"

#include <3ds.h>

struct Texture
{
    C2D_SpriteSheet sheet;
    C2D_Image image;

    Texture(const char* path)
    {
        this->sheet = C2D_SpriteSheetLoad(path);
        this->image = C2D_SpriteSheetGetImage(this->sheet, 0);
    }

    ~Texture()
    {
        if (this->sheet != NULL)
            C2D_SpriteSheetFree(this->sheet);
    }

    void render(float x, float y, Quad& quad)
    {
        C2D_DrawParams params;

        Viewport viewport            = quad.viewport;
        Tex3DS_SubTexture subtexture = quad.CalculateViewport(viewport, this->image.tex);

        this->image.subtex = &subtexture;

        params.pos    = { x, y, (float)viewport.w, (float)viewport.h };
        params.depth  = 0;
        params.angle  = 0.0f;
        params.center = { 0.0f, 0.0f };

        C2D_DrawImage(this->image, &params);
    }

    uint16_t width()
    {
        return this->image.subtex->width;
    }

    uint16_t height()
    {
        return this->image.subtex->height;
    }

    uint getPixel(uint x, uint y)
    {
        uint16_t _width = this->image.tex->width;
        uint index      = coordToIndex(_width, x + 1, y + 1);

        return ((u32*)this->image.tex->data)[index];
    }

    void setPixel(uint x, uint y, float r, float g, float b, float a)
    {
        uint16_t _width = this->image.tex->width;
        uint index      = coordToIndex(_width, x + 1, y + 1);

        ((u32*)this->image.tex->data)[index] = setPixelColors(r, g, b, a);
    }
};
