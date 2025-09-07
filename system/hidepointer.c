#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>

/* 16×16 completely transparent pointer (mask + image, 2 words per row) */
static UWORD nullPointer[16 * 2] = {0};

struct Screen *openScreen(void) {
    return OpenScreenTags(NULL,
                          SA_Depth, 4, /* 16 colors */
                          SA_Width, 320,
                          SA_Height, 256,
                          SA_Title, (ULONG) "Hide Pointer",
                          SA_ShowTitle, FALSE,
                          TAG_END);
}

struct Window *openHiddenWindow(struct Screen *scr) {
    return OpenWindowTags(NULL,
                          WA_CustomScreen, scr,
                          WA_Left, 0,
                          WA_Top, 0,
                          WA_Width, 1,
                          WA_Height, 1,
                          WA_Backdrop, TRUE, /* attaches to screen */
                          WA_Borderless, TRUE,
                          WA_SimpleRefresh, TRUE,
                          WA_BackFill, LAYERS_NOBACKFILL,
                          WA_Activate, FALSE,
                          TAG_END);
}

void cleanUp(struct Screen *screen, struct Window *window) {
    if (window != NULL) {
        ClearPointer(window);
        CloseWindow(window);
    }
    if (screen != NULL) {
        CloseScreen(screen);
    }
}

int main() {
    struct Screen *screen = NULL;
    struct Window *window = NULL;

    // Open a screen
    if (NULL == (screen = openScreen())) {
        printf("Failed to open screen!\n");
        return RETURN_FAIL;
    }
    // Open a hidden window on that screen
    if (NULL == (window = openHiddenWindow(screen))) {
        printf("Failed to open hidden window!\n");
        CloseScreen(screen);
        return RETURN_FAIL;
    }

    // Hide pointer with a transparent 16×16 image
    SetPointer(window, nullPointer, 16, 16, 0, 0);

    /*
     * Attention: Moving the mouse will show the pointer again!
     * One way to hide it completely is to take over the system
     * with SystemControl(SCON_TakeOverSys, TRUE, TAG_DONE);
     * But this will freeze the whole system, so use with care!
     * Don't forget to release the system again with SystemControl(SCON_TakeOverSys, FALSE, TAG_DONE);
     * before clear the pointer and closing the window/screen.
     */

    // Just wait a few seconds for demonstration purposes
    Delay(5 * 50); /* wait ~5 seconds */

    // Show pointer again
    ClearPointer(window);

    // Cleanup
    cleanUp(screen, window);

    return RETURN_OK;
}
