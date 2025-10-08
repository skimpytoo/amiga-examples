#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <graphics/view.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <devices/timer.h>
#include <string.h>
#include <stdio.h>
#include <exec/execbase.h>
#include <proto/exec.h>
#include <proto/timer.h>

#define WIDTH 320
#define HEIGHT 256
#define DEPTH 7 /* 7 bitplanes = 128 colors */

struct FPSCounter {
    ULONG timer;      // timer counter up to 1 second
    WORD fpsValue;    //  Actual fps value
    WORD frameCount;  // Number of frames rendered
};

struct GfxBase *GfxBase;
struct Library *TimerBase;
static struct IORequest timereq;
struct FPSCounter fpsCounter;
struct Screen *screen = NULL;
struct ScreenBuffer *screenBufferOne = NULL, *screenBufferTwo = NULL;
struct ColorMap *colorMap;

ULONG getElapsedTime(void) {
    static struct timeval tt;
    struct timeval a, b;

    GetSysTime(&a);
    b = a;
    SubTime(&b, &tt);
    tt = a;

    return b.tv_secs * 1000 + b.tv_micro / 1000;
}

void calcFps(struct FPSCounter *fpsCounter) {
    fpsCounter->timer += getElapsedTime();
    if (fpsCounter->timer > 1000) {
        fpsCounter->fpsValue = fpsCounter->frameCount - 1;  // -1 because we increment frameCount after the last frame is drawn
        fpsCounter->frameCount = 0;
        fpsCounter->timer -= 1000;
    }
}

int isLeftMouseNotButtonPressed(void) {
    return (*(UBYTE *)0xBFE001) & 0x40;
}

void updateFpsDisplay(struct FPSCounter *fpsCounter, struct RastPort *drawRastPort) {
    char buf[10];
    calcFps(fpsCounter);
    sprintf(buf, "%d fps", fpsCounter->fpsValue);
    SetAPen(drawRastPort, 1);
    Move(drawRastPort, 10, 10);
    Text(drawRastPort, buf, strlen(buf));
    fpsCounter->frameCount++;
}

void cleanUp(void) {
    if (screenBufferOne) FreeScreenBuffer(screen, screenBufferOne);
    if (screenBufferTwo) FreeScreenBuffer(screen, screenBufferTwo);
    if (colorMap) FreeColorMap(colorMap);
    if (screen) CloseScreen(screen);
    // Close the timer device
    CloseDevice(&timereq);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
}

int main(void) {
    struct RastPort rastPortOne, rastPortTwo;
    struct ScreenBuffer *activeBuffer, *hiddenBuffer;
    struct RastPort *drawRastPort;
    struct ScreenBuffer *tmpBuffer;

    ULONG pal[] = {
        0x00000009, /* 9 entries follow */
        0x00000000, /* 0 black */
        0x00ffffff, /* 1 white */
        0x00ff0000, /* 2 red */
        0x0000ff00, /* 3 green */
        0x000000ff, /* 4 blue */
        0x00ffff00, /* 5 yellow */
        0x0000ffff, /* 6 cyan */
        0x00ff00ff, /* 7 magenta */
        0x00808080  /* 8 gray */
    };

    ULONG frame = 0;
    ULONG timer = 0;

    OpenDevice("timer.device", 0, &timereq, 0);
    TimerBase = (struct Library *)timereq.io_Device;
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39);
    if (!GfxBase) return 20;
    printf("Opened graphics.library\n");
    screen = OpenScreenTags(NULL,
                            SA_Width, WIDTH,
                            SA_Height, HEIGHT,
                            SA_Depth, DEPTH,
                            SA_DisplayID, LORES_KEY,
                            SA_Type, CUSTOMSCREEN,
                            SA_ShowTitle, FALSE,
                            SA_Quiet, TRUE,
                            TAG_END);
    if (!screen) cleanUp();
    printf("Opened screen\n");
    // Set up the color palette
    colorMap = GetColorMap(1 << DEPTH);
    if (colorMap) {
        screen->ViewPort.ColorMap = colorMap;
        LoadRGB32(&screen->ViewPort, pal);
    }
    printf("Set up colormap\n");
    screenBufferOne = AllocScreenBuffer(screen, NULL, SB_COPY_BITMAP);
    screenBufferTwo = AllocScreenBuffer(screen, NULL, SB_COPY_BITMAP);
    if (!screenBufferOne || !screenBufferTwo) cleanUp();
    printf("Allocated screen buffers\n");
    InitRastPort(&rastPortOne);
    rastPortOne.BitMap = screenBufferOne->sb_BitMap;
    rastPortOne.Font = screen->RastPort.Font;
    printf("Initialized rastPortOne\n");
    InitRastPort(&rastPortTwo);
    rastPortTwo.BitMap = screenBufferTwo->sb_BitMap;
    rastPortTwo.Font = screen->RastPort.Font;
    printf("Initialized rastPortTwo\n");
    activeBuffer = screenBufferOne;
    hiddenBuffer = screenBufferTwo;
    SetAPen(&rastPortOne, 3);
    RectFill(&rastPortOne, 50, 50, 100, 100);
    SetAPen(&rastPortTwo, 4);
    RectFill(&rastPortTwo, 150, 150, 200, 200);
    // init timer
    getElapsedTime();
    printf("Starting main loop\n");
    while (isLeftMouseNotButtonPressed()) {
        drawRastPort = (hiddenBuffer == screenBufferOne) ? &rastPortOne : &rastPortTwo;

        updateFpsDisplay(&fpsCounter, drawRastPort);

        /* Flip as soon as safe */
        while (!ChangeScreenBuffer(screen, hiddenBuffer)) {
            WaitBOVP(&screen->ViewPort);
        }
        tmpBuffer = activeBuffer;
        activeBuffer = hiddenBuffer;
        hiddenBuffer = tmpBuffer;
        WaitTOF();
        frame++;
    }
    cleanUp();
}