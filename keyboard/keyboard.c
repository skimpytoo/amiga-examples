#include <clib/lowlevel_protos.h>
#include <clib/exec_protos.h>
#include <CLIB/dos_protos.h>
#include <stdio.h>
#include <stdlib.h>

#define ESC_KEY 69
#define LEFT_KEY 0x4f
#define RIGHT_KEY 0x4e
#define UP_KEY 0x4c
#define DOWN_KEY 0x4d

int main(int argc, char** argv) {
    ULONG keyboard;
    ULONG rawCode;
    ULONG keyPressed = 0;
    ULONG prevKeyboard = 0;
    struct Library* LowLevelBase = NULL;

    // Open lowlevel library to access joypad functions
    if (!(LowLevelBase = OpenLibrary("lowlevel.library", 0L))) {
        printf("Couldn't open lowlevel.library\n");
        exit(RETURN_FAIL);
    }

    printf("Test keyboard input. Press arrow keys and/or modifier keys, or ESC to exit\n");

    do {
        // Read the current keyboard state. lowlevel.library GetKey()
        keyboard = GetKey();
        keyPressed = 0;
        // Only print when state changes
        if (keyboard != prevKeyboard) {
            // Extract raw key code (lower byte)
            rawCode = keyboard & 0x00ff;
            // Check for modifier keys (upper byte)
            if (keyboard & LLKF_LSHIFT) {
                keyPressed = 1;
                printf("Left shift");
            }
            if (keyboard & LLKF_RSHIFT) {
                keyPressed = 1;
                printf("Right shift");
            }
            if (keyboard & LLKF_CAPSLOCK) {
                keyPressed = 1;
                printf("Caps lock");
            }
            if (keyboard & LLKF_CONTROL) {
                keyPressed = 1;
                printf("Control");
            }
            if (keyboard & LLKF_LALT) {
                keyPressed = 1;
                printf("Left alt");
            }
            if (keyboard & LLKF_RALT) {
                keyPressed = 1;
                printf("Right alt");
            }
            if (keyboard & LLKF_LAMIGA) {
                keyPressed = 1;
                printf("Left amiga");
            }
            if (keyboard & LLKF_RAMIGA) {
                keyPressed = 1;
                printf("Right amiga");
            }
            // Check for key codes
            if (rawCode == UP_KEY) {
                keyPressed = 1;
                printf("Up");
            }
            if (rawCode == DOWN_KEY) {
                keyPressed = 1;
                printf("Down");
            }
            if (rawCode == LEFT_KEY) {
                keyPressed = 1;
                printf("Left");
            }
            if (rawCode == RIGHT_KEY) {
                keyPressed = 1;
                printf("Right");
            }
            if (keyPressed) {
                printf("\n");
            }
            // Update previous state
            prevKeyboard = keyboard;
        }
        // Continue until ESC key is pressed
    } while ((keyboard & 0xff) != ESC_KEY);

    CloseLibrary((struct Library*)LowLevelBase);
    return RETURN_OK;
}
