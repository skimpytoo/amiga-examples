#include <exec/types.h>
#include <proto/dos.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/sprite.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <libraries/dos.h>
#include <exec/memory.h>

#include <clib/graphics_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_stdio_protos.h>
#include <clib/lowlevel_protos.h>

#include <stdlib.h>

struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_DEPTH 7

/**
 * @brief
 *
 * @param screen Screen where the fading should happen.
 * @param startPalette Starting color palette (fade from) - LoadRGB32 format
 * @param endPalette Ending color palette (fade to) - LoadRGB32 format
 * @param partitions How fine the fading steps should be. The more steps the finer the fading
 * @param steps How much steps should be taken. If steps==partitions a full fade from startPalette to endPalette is done
 */
void fade(struct Screen *screen, ULONG *startPalette, ULONG *endPalette, int partitions, int steps);

int main(void) {
    struct Screen *screen;
    int return_code = RETURN_OK;
    ULONG *screenColorPalette = NULL;
    ULONG *blackPalette = NULL;

    // +2 for LoadRGB32 format: [0] = number of colors, [last] = terminationMarker (0)
    // BlackPalette all values to 0 -> black. Endpallete for fade to black
    blackPalette = (ULONG *)AllocMem(sizeof(ULONG) * (1 << SCREEN_DEPTH + 2), MEMF_ANY | MEMF_CLEAR);
    // Actual screen palette
    screenColorPalette = (ULONG *)AllocMem(sizeof(ULONG) * (1 << SCREEN_DEPTH + 2), MEMF_ANY | MEMF_CLEAR);

    if (blackPalette != NULL && screenColorPalette != NULL) {
        if (NULL == (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 37L))) {
            return_code = RETURN_FAIL;
        } else {
            if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37L))) {
                return_code = RETURN_FAIL;
            } else {
                if (NULL == (screen = OpenScreenTags(NULL,
                                                     SA_Left, 0,
                                                     SA_Top, 0,
                                                     SA_Width, SCREEN_WIDTH,
                                                     SA_Height, SCREEN_HEIGHT,
                                                     SA_Depth, SCREEN_DEPTH,
                                                     SA_Title, NULL,
                                                     SA_Quiet, TRUE,
                                                     SA_ShowTitle, FALSE,
                                                     TAG_DONE))) {
                    return_code = RETURN_FAIL;
                } else {
                    // Some random color
                    SetRGB4(&screen->ViewPort, 21, 12, 3, 8);
                    SetRGB4(&screen->ViewPort, 22, 13, 13, 13);
                    SetRGB4(&screen->ViewPort, 23, 4, 4, 15);

                    // Some random rectangeles
                    SetAPen(&screen->RastPort, 21);
                    RectFill(&screen->RastPort, 20, 20, 40, 30);

                    SetAPen(&screen->RastPort, 22);
                    RectFill(&screen->RastPort, 80, 80, 140, 160);

                    SetAPen(&screen->RastPort, 23);
                    RectFill(&screen->RastPort, 100, 100, 180, 190);

                    // Copy colortable into colors-array
                    GetRGB32(screen->ViewPort.ColorMap, 0, 1 << SCREEN_DEPTH, &screenColorPalette[1]);

                    while (GetKey() != 69) {  // ESC to terminate
                        // Fade to black
                        fade(screen, screenColorPalette, blackPalette, 80, 80);
                        // Fadeing back
                        fade(screen, blackPalette, screenColorPalette, 80, 80);
                    }
                    CloseScreen(screen);
                }
                CloseLibrary((struct Library *)IntuitionBase);
            }
            CloseLibrary((struct Library *)GfxBase);
        }
    }

    if (blackPalette != NULL) {
        FreeMem(blackPalette, sizeof(ULONG) * (1 << SCREEN_DEPTH + 2));
    }
    if (screenColorPalette != NULL) {
        FreeMem(screenColorPalette, sizeof(ULONG) * (1 << SCREEN_DEPTH + 2));
    }

    exit(return_code);
}
/**
 * @brief Fade from startpal to endpal. If partition != steps no full fade to endPal will be done
 *
 * @param screen
 * @param startColPal Pallete from which we want to fade (LoadRGB32 format)
 * @param endColPal  Pallete to wich we want to fade (LoadRGB32 format)
 * @param partitions Number of steps between startPal and endPal
 * @param steps Number of fading steps.
 */
void fade(struct Screen *screen, ULONG *startColPal, ULONG *endColPal, int partitions, int steps) {
    static long *tempPal = NULL;  // temporal copy of palette
    static long *value = NULL;    // Colordiff between each fading step
    static ULONG *rgbPal = NULL;  // TempPal in LoadRGB32() format
    long tmp;
    int e, i;

    int colors = 1 << screen->BitMap.Depth;
    int loadRgb32ArraySize = colors * 3 + 2;  // [0]+colors(r+g+b)+[terminationMarker]

    // more steps than partitions is of no use
    if (steps > partitions) {
        steps = partitions;
    }

    tempPal = (long *)AllocMem(sizeof(long) * loadRgb32ArraySize, MEMF_ANY);
    value = (long *)AllocMem(sizeof(long) * loadRgb32ArraySize, MEMF_ANY);
    rgbPal = (ULONG *)AllocMem(sizeof(ULONG) * loadRgb32ArraySize, MEMF_ANY);

    if (tempPal != NULL && value != NULL && rgbPal != NULL) {
        rgbPal[0] = colors << 16;            // LoadRGB32 Format: First element. Number of colors
        rgbPal[loadRgb32ArraySize - 1] = 0;  // LoadRGB32 Format: Last element. terminationMarker. Zero

        for (e = 1; e < (loadRgb32ArraySize - 1); e++) {
            tempPal[e] = (long)((startColPal[e] >> 16) & 65535L);  // Initialize temporal colortable
            tmp = (long)((endColPal[e] >> 16) & 65535L);
            value[e] = (long)((tmp - tempPal[e]) / partitions);  // Claculate colorvalue increase
        }

        for (e = 0; e < steps; e++)  // Calculate all given steps
        {
            for (i = 1; i < (loadRgb32ArraySize - 1); i++)  // Iterate through all RGB values
            {
                tempPal[i] = tempPal[i] + value[i];                 // Add fading step difference
                rgbPal[i] = (((ULONG)(tempPal[i]) << 16) | 65536);  // Convert it into LoadRGB32 value
            }
            LoadRGB32(&screen->ViewPort, rgbPal);
            WaitTOF();
        }
        if (steps == partitions) {
            LoadRGB32(&screen->ViewPort, endColPal);  // Finished: load endPal to avoid rounding errors
        }
    }
    if (tempPal != NULL) {
        FreeMem(tempPal, sizeof(long) * loadRgb32ArraySize);
    }
    if (value != NULL) {
        FreeMem(value, sizeof(long) * loadRgb32ArraySize);
    }
    if (rgbPal != NULL) {
        FreeMem(rgbPal, sizeof(ULONG) * loadRgb32ArraySize);
    }
}