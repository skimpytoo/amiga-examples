/* Simple joypad polling example using lowlevel.library.
 * Demonstrates: open libs, read controller state, ESC to exit, WaitTOF to pace the loop.
 */
#include <clib/lowlevel_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/dos_protos.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 1
#define ESC_KEY 69

ULONG readJoyPad(BYTE port) {
    BOOL isJoypadUsed = FALSE;

    // Read the current state of the joypad
    ULONG state = ReadJoyPort(port);

    if (state & JPF_BUTTON_RED) {
        printf("Red button");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_BUTTON_BLUE) {
        printf("Blue button");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_BUTTON_GREEN) {
        printf("Green button");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_BUTTON_YELLOW) {
        printf("Yellow button");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_JOY_UP) {
        printf("Up");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_JOY_DOWN) {
        printf("Down");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_JOY_LEFT) {
        printf("Left");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_JOY_RIGHT) {
        printf("Right");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_BUTTON_PLAY) {
        printf("Play");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_BUTTON_FORWARD) {
        printf("Forward");
        isJoypadUsed = TRUE;
    }
    if (state & JPF_BUTTON_REVERSE) {
        printf("Reverse");
        isJoypadUsed = TRUE;
    }
    if (!isJoypadUsed) {
        return 0;  // no input detected
    }
    return state;
}

int main(int argc, char** argv) {
    ULONG controller;
    struct GfxBase* GfxBase = NULL;
    struct Library* LowLevelBase = NULL;

    // Open graphics library as WaitTOF() is used in the main loop
    if (!(GfxBase = (struct GfxBase*)OpenLibrary("graphics.library", 37L))) {
        printf("Couldn't open graphics.library\n");
        exit(RETURN_FAIL);
    }
    // Open lowlevel library to access joypad functions
    if (!(LowLevelBase = OpenLibrary("lowlevel.library", 0L))) {
        printf("Couldn't open lowlevel.library\n");
        CloseLibrary((struct Library*)GfxBase);
        exit(RETURN_FAIL);
    }
    // We override the default auto-detection and force the system to use a game controller.
    // Just for demonstration purposes. Normally you would use auto-detection.
    if (FALSE == SetJoyPortAttrs(PORT, SJA_TYPE_GAMECTLR)) {
        printf("Failed to set joyport attributes\n");
    };

    printf("Test joypad on port: %d. Press ESC to exit\n", PORT);
    while (GetKey() != ESC_KEY) {  // ESC to terminate
        controller = readJoyPad(PORT);
        if (controller != 0) {
            printf("\n");
        }
        // Small delay to avoid flooding the output
        WaitTOF();
    }

    // Restore auto-detection after forcing game controller mode
    SetJoyPortAttrs(PORT, SJA_Reinitialize);

    CloseLibrary((struct Library*)GfxBase);
    CloseLibrary((struct Library*)LowLevelBase);
    return RETURN_OK;
}
