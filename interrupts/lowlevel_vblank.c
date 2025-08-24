#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <clib/lowlevel_protos.h>
#include <clib/timer_protos.h>
#include <exec/types.h>
#include <exec/interrupts.h>
#include <exec/memory.h>
#include <exec/exec.h>
#include <hardware/intbits.h>
#include <proto/exec.h>
#include <proto/dos.h>

#define VBLANKS_PER_SECOND 50  // On a PAL system, there are 50 VBLANKs per second

/* ----- Own data structure to pass information to intervall ----- */
struct MyCustomData {
    ULONG frameCount;  // Counts the number of frames
    ULONG seconds;     // Counts the number of seconds
};

struct MyCustomData vbData;
/* ----- Interrupt-Service-Routine ----- */
__saveds __interrupt ULONG MyVBlankISR(void) {
    struct MyCustomData *data = &vbData;
    data->frameCount++;

    // A example: Calc the seconds based on the frame count
    // This assumes that the VBlank ISR is called at a fixed rate (50 times per second)
    // and increments the seconds counter every 50 frames.
    if ((data->frameCount % VBLANKS_PER_SECOND) == 0) {
        data->seconds++;
        // Here you could do things like updating game logic, settings flags for the main routine.
    };

    return 0;
};

int main(void) {
    ULONG start;
    struct Library *LowLevelBase = NULL;
    APTR intHandle = NULL;

    if (!(LowLevelBase = OpenLibrary("lowlevel.library", 0L))) {
        printf("Couldn't open lowlevel.library\n");
        return RETURN_FAIL;
    }

    // Initialize the VBlank custom data structure
    vbData.frameCount = 0;
    vbData.seconds = 0;

    // Add the VBlank interrupt. Function to call and data
    intHandle = AddVBlankInt(MyVBlankISR, (APTR)&vbData);

    /* Main loop: We wait for 5 seconds */
    start = vbData.frameCount;
    while (vbData.frameCount - start < 5 * VBLANKS_PER_SECOND) {
        /* Do your things: rendering, wait, read input,...*/
    }

    // Remove the interrupt
    RemVBlankInt(intHandle);
    // Close the lowlevel.library
    if (LowLevelBase) {
        CloseLibrary(LowLevelBase);
    }
    // Finally, print the results
    printf("Frames: %lu, seconds: %lu\n",
           vbData.frameCount, vbData.seconds);

    return RETURN_OK;
}
