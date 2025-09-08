#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>
#include <proto/intuition.h>
#include <dos/dos.h>

#include <stdio.h>
#include <string.h>

#include "includes/iff.h"

// Filename and path to the example image
#define EXAMPLE_FILE "assets/example.iff"

struct Library *IFFBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Screen *myscreen = NULL;

/**
 * @brief Loads and displays the image
 *
 * @param filename Name/Path of image file
 * @return pointer to screen with image
 */
struct Screen *DisplayILBM(char *filename) {
    IFFL_HANDLE iffHandle = NULL;
    LONG numOfColors;
    UWORD colortable[256];
    struct IFFL_BMHD *bmhd;

    // Open the iff file
    if (NULL == (iffHandle = IFFL_OpenIFF(filename, IFFL_MODE_READ))) {
        printf("Can't open file '%s'\n", filename);
        return NULL;
    }

    // Receive the bitmap header
    if (NULL == (bmhd = IFFL_GetBMHD(iffHandle))) {
        printf("This file has no bitmap header.\n");
        IFFL_CloseIFF(iffHandle);
        return NULL;
    }
    // Open a custom screen with the size and depth of the image
    if (myscreen = OpenScreenTags(NULL,
                                  SA_Left, 0,
                                  SA_Top, 0,
                                  SA_Type, CUSTOMSCREEN,
                                  SA_Width, bmhd->w,
                                  SA_Height, bmhd->h,
                                  SA_Depth, bmhd->nPlanes,
                                  SA_Title, NULL,
                                  SA_Quiet, TRUE,
                                  SA_Behind, TRUE,
                                  SA_ShowTitle, FALSE,
                                  TAG_DONE)) {
        // Get the colortable of the image
        numOfColors = IFFL_GetColorTab(iffHandle, (WORD *)colortable);
        // Load the colortable into the screen's viewport
        LoadRGB4(&(myscreen->ViewPort), colortable, numOfColors);
        // Try to decode the picture into the screen's bitmap
        if (IFFL_DecodePic(iffHandle, &myscreen->BitMap)) {
            return myscreen;
        } else {
            printf("Can't decode picture.\n");
        }
    } else {
        printf("Can't open screen.\n");
    }
    IFFL_CloseIFF(iffHandle);
    return myscreen;
}

/**
 * @brief Closes all opened libraries and screen
 */
void closeAll() {
    if (myscreen != NULL) {
        CloseScreen(myscreen);
    }
    if (IFFBase != NULL) {
        CloseLibrary(IFFBase);
    }
    if (IntuitionBase != NULL) {
        CloseLibrary((struct Library *)IntuitionBase);
    }
    if (GfxBase != NULL) {
        CloseLibrary((struct Library *)GfxBase);
    }
}

int main(void) {
    if (NULL == (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0L))) {
        printf("Can't open graphics.library\n");
        return RETURN_FAIL;
    }
    if (NULL == (IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0L))) {
        printf("Can't open intuition.library\n");
        closeAll();
        return RETURN_FAIL;
    }
    if (NULL == (IFFBase = OpenLibrary(IFFNAME, IFFVERSION))) {
        printf("Can't open iff.library V%ld+\n", IFFVERSION);
        closeAll();
        return RETURN_FAIL;
    }

    if (NULL == DisplayILBM(EXAMPLE_FILE)) {
        printf("Displaying image failed.\n");
        closeAll();
        return RETURN_FAIL;
    }
    ScreenToFront(myscreen);
    printf("Press Ctrl-C to quit.\n");
    Wait(SIGBREAKF_CTRL_C);

    closeAll();
    return RETURN_OK;
}
