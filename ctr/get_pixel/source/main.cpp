/*
    Hello World example made by Aurelio Mannara for libctru
    This code was modified for the last time on: 12/12/2014 21:00 UTC+1
*/

#include <3ds.h>
#include <stdio.h>
#include <vector>

#include <citro2d.h>
#include <citro3d.h>

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cstdlib> // for rand() and RAND_MAX
#include <math.h>

static bool g_inFrame = false;
static C3D_RenderTarget* g_currentTarget;

static std::vector<C3D_RenderTarget*> g_targets(3);

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

#include "color.h"
#include "quad.h"
#include "texture.h"

void ensureInFrame()
{
    if (!g_inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        g_inFrame = true;
    }
}

void bindFrameBuffer(C3D_RenderTarget* target)
{
    ensureInFrame();

    g_currentTarget = target;
    C2D_SceneBegin(g_currentTarget);
}

void clearScreen(const Color& color)
{
    C2D_TargetClear(g_currentTarget, C2D_Color32f(color.r, color.g, color.b, color.a));
}

void presentFrameBuffer()
{
    if (g_inFrame)
    {
        C3D_FrameEnd(0);
        g_inFrame = false;
    }
}

int main(int argc, char** argv)
{
    gfxInitDefault();

    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
        svcBreak(USERBREAK_PANIC);

    if (!C2D_Init(C2D_DEFAULT_MAX_OBJECTS))
        svcBreak(USERBREAK_PANIC);

    C2D_Prepare();

    consoleInit(GFX_BOTTOM, NULL);

    g_targets = { C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT),
                  C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT) };

    romfsInit();

    const Color clearColor;

    Texture texture("romfs:/graphics/texture.t3x");
    Quad quad(Viewport { 0, 0, 32, 32 }, 32, 32);

    FILE* file = fopen("colors.txt", "w");

    for (uint y = 0; y < texture.height(); y++)
    {
        for (uint x = 0; x < texture.width(); x++)
        {
            uint pixelColor = texture.getPixel(x, y);
            Color gotColor  = getPixelColors(pixelColor);

            char buffer[2];
            sprintf(buffer, "%s", gotColor.colorName());
            fwrite(buffer, 1, strlen(buffer), file);
        }
        fwrite("\n", 1, strlen("\n"), file);
    }

    fclose(file);

    for (uint y = 0; y < texture.height() / 2; y++)
    {
        for (uint x = 0; x < texture.width() / 2; x++)
            texture.setPixel(x, y, 1, 1, 1, 1);
    }

    // Main loop
    while (aptMainLoop())
    {
        hidScanInput();

        uint32_t key = hidKeysDown();
        if (key & KEY_START)
            break;

        for (auto fb : g_targets)
        {
            bindFrameBuffer(fb);
            clearScreen(clearColor);

            texture.render((SCREEN_WIDTH - texture.width()) * 0.5,
                           (SCREEN_HEIGHT - texture.height()) * 0.5, quad);

            presentFrameBuffer();
        }
    }

    romfsExit();

    C2D_Fini();

    C3D_Fini();

    gfxExit();

    return 0;
}
