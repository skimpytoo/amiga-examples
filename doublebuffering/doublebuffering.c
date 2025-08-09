#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/execbase.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <devices/timer.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/timer.h>

#define WIDTH 320
#define HEIGHT 256 * 2 + 64
#define DEPTH 5

struct FPSCounter {
    ULONG timer;      // timer counter up to 1 second
    WORD fpsValue;    //  Actual fps value
    WORD frameCount;  // Number of frames rendered
};

extern struct ExecBase *SysBase;
struct GfxBase *GfxBase;
struct Library *TimerBase;
static struct IORequest timereq;
struct FPSCounter fpsCounter;

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

int isLeftMouseButtonPressed(void) {
    return (*(UBYTE *)0xBFE001) & 0x40;
}

int main(void) {
    struct DimensionInfo dimsinfo;

    struct View view, *oldview = NULL;
    struct ViewPort viewPort;

    struct BitMap *bufferOneBitMap;
    struct BitMap *bufferTwoBitMap;
    struct RastPort bufferOneRastPort;
    struct RastPort bufferTwoRastPort;
    struct RastPort *actualDrawRastPort;

    struct RasInfo rasInfo;

    struct cprlist *LOCpr1;
    struct cprlist *SHCpr1;
    struct cprlist *LOCpr2;
    struct cprlist *SHCpr2;

    LONG frame = 0;
    UBYTE numstr[16];

    OpenDevice("timer.device", 0, &timereq, 0);
    TimerBase = (struct Library *)timereq.io_Device;

    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);

    GetDisplayInfoData(FindDisplayInfo(LORES_KEY), (UBYTE *)&dimsinfo,
                       sizeof(struct DimensionInfo), DTAG_DIMS,
                       NULL);

    // Save the current view to restore later
    oldview = GfxBase->ActiView;

    // Initialize a new view structure
    InitView(&view);

    // Allocate bitmaps for double buffering
    bufferOneBitMap = AllocBitMap(WIDTH, HEIGHT, DEPTH, BMF_CLEAR | BMF_DISPLAYABLE, NULL);
    bufferTwoBitMap = AllocBitMap(WIDTH, HEIGHT, DEPTH, BMF_CLEAR | BMF_DISPLAYABLE, bufferOneBitMap);

    // Initialize the bitmaps and rastports
    InitBitMap(bufferOneBitMap, DEPTH, WIDTH, HEIGHT);
    InitBitMap(bufferTwoBitMap, DEPTH, WIDTH, HEIGHT);

    InitRastPort(&bufferOneRastPort);
    // Link the rastport to the first bitmap
    bufferOneRastPort.BitMap = bufferOneBitMap;
    // Set all pixels in rastport to black (color index 0)
    SetRast(&bufferOneRastPort, 0);

    InitRastPort(&bufferTwoRastPort);
    // Link the rastport to the second bitmap
    bufferTwoRastPort.BitMap = bufferTwoBitMap;
    // Set all pixels in rastport to black (color index 0)
    SetRast(&bufferTwoRastPort, 0);

    // Initialize rasInfo (link the viewport to the bitmap)
    rasInfo.BitMap = bufferOneBitMap;
    rasInfo.RxOffset = 0;
    rasInfo.RyOffset = 0;
    rasInfo.Next = NULL;

    // Initialize the viewPort for the screen handling
    InitVPort(&viewPort);
    view.ViewPort = &viewPort;
    viewPort.RasInfo = &rasInfo;
    viewPort.DWidth = WIDTH;
    viewPort.DHeight = HEIGHT;

    // Generate the copper lists for the viewPort
    MakeVPort(&view, &viewPort);
    // Merge all copperlist instructions into the view
    MrgCop(&view);

    LOCpr1 = view.LOFCprList;
    SHCpr1 = view.SHFCprList;

    view.LOFCprList = 0;
    view.SHFCprList = 0;

    rasInfo.BitMap = bufferTwoBitMap;

    MakeVPort(&view, &viewPort);

    MrgCop(&view);

    LOCpr2 = view.LOFCprList;
    SHCpr2 = view.SHFCprList;

    // Use the new view
    LoadView(&view);

    // init timer
    getElapsedTime();
    while (isLeftMouseButtonPressed()) {
        // Wait for the beginning of the next video frame
        WaitTOF();
        // Now we swap the frame buffers
        if (frame == 0) {
            view.LOFCprList = LOCpr1;
            view.SHFCprList = SHCpr1;
            actualDrawRastPort = &bufferOneRastPort;
        } else {
            view.LOFCprList = LOCpr2;
            view.SHFCprList = SHCpr2;
            actualDrawRastPort = &bufferTwoRastPort;
        }

        calcFps(&fpsCounter);
        // Copy fpsvalue to the stringbuffer to display
        sprintf(numstr, "%d fps", fpsCounter.fpsValue);

        // Draw fps value on the screen
        SetAPen(actualDrawRastPort, 31);
        Move(actualDrawRastPort, 10, 10);
        Text(actualDrawRastPort, numstr, strlen(numstr));

        // Actualize the view
        LoadView(&view);
        // Swap the frame buffers
        frame ^= 1;
        fpsCounter.frameCount++;
    };

    // Restore the old view
    LoadView(oldview);

    // Wait for the end of the current video frame. So it is save to free the resources
    WaitTOF();

    // Free all copper list ressources
    FreeCprList(LOCpr1);
    FreeCprList(LOCpr2);
    FreeCprList(SHCpr1);
    FreeCprList(SHCpr2);

    // Free the viewPort and the view
    FreeVPortCopLists(&viewPort);

    // Free the bitmaps
    FreeBitMap(bufferOneBitMap);
    FreeBitMap(bufferTwoBitMap);

    // Close the timer device
    CloseDevice(&timereq);

    if (GfxBase)
        CloseLibrary((struct Library *)GfxBase);

    return 0;
}
