#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <clib/lowlevel_protos.h>
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
__saveds __interrupt BOOL MyVBlankISR(void) {
    // struct MyCustomData *data = (struct MyCustomData *)VBlankInterrupt.is_Data;
    struct MyCustomData *data = &vbData;
    data->frameCount++;

    // A example: Calc the seconds based on the frame count
    // This assumes that the VBlank ISR is called at a fixed rate (50 times per second)
    // and increments the seconds counter every 50 frames.
    if ((data->frameCount % VBLANKS_PER_SECOND) == 0) {
        data->seconds++;
        // Here you could do things like updating game logic, settings flags for the main routine.
    };
    return TRUE;
}

int main(void) {
    struct Library *LowLevelBase = NULL;
    APTR intHandle = NULL;

    if (!(LowLevelBase = OpenLibrary("lowlevel.library", 0L))) {
        printf("Couldn't open lowlevel.library\n");
        return RETURN_FAIL;
    }

    // Initialize the interrupt custom data structure
    vbData.frameCount = 0;
    vbData.seconds = 0;

    // Add the timer interrupt. Function to call and data
    intHandle = AddTimerInt(MyVBlankISR, (APTR)&vbData);

    // Start a continuous timer interrupt at the rate of VBLANKS_PER_SECOND (50 for PAL systems )
    StartTimerInt(intHandle, 20000, TRUE);

    while (vbData.seconds < 5) {
        /* Do your things: rendering, wait, read input,...*/
    }
    // Stop the continuous timer interrupt
    StopTimerInt(intHandle);
    // Remove the interrupt
    RemTimerInt(intHandle);
    // Close the lowlevel.library
    if (LowLevelBase) {
        CloseLibrary(LowLevelBase);
    }
    // Finally, print the results
    printf("Frames: %lu, seconds: %lu\n",
           vbData.frameCount, vbData.seconds);

    return 0;
}
