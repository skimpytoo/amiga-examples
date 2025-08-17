#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exec/types.h>
#include <exec/interrupts.h>
#include <exec/memory.h>
#include <exec/exec.h>
#include <hardware/intbits.h>
#include <proto/exec.h>
#include <proto/dos.h>

#define VBLANKS_PER_SECOND 50  // On a PAL system, there are 50 VBLANKs per second

/* ----- Own Data structure to pass information to intervall ----- */
struct MyCustomData {
    ULONG frameCount;  // Counts the number of frames
    ULONG seconds;     // Counts the number of seconds
};

struct Interrupt VBlankInterrupt;

/* ----- Interrupt-Service-Routine ----- */
__saveds __interrupt ULONG MyVBlankISR(void) {
    struct MyCustomData *data = (struct MyCustomData *)VBlankInterrupt.is_Data;

    data->frameCount++;

    // A example: Calc the seconds based on the frame count
    // This assumes that the VBlank ISR is called at a fixed rate (50 times per second)
    // and increments the seconds counter every 50 frames.
    if ((data->frameCount % VBLANKS_PER_SECOND) == 0) {
        data->seconds++;
        // Here you could do things like updating game logic, settings flags for the main routine.
    };

    return 0;
}

int main(void) {
    struct MyCustomData vbData;
    ULONG start;

    // Initialize the VBlank custom data structure
    vbData.frameCount = 0;
    vbData.seconds = 0;

    // Initialize the VBlank interrupt structure
    // Point to the interrupt service routine to execute on VBlank
    VBlankInterrupt.is_Code = (VOID (*)())MyVBlankISR;
    // Set the data pointer to our custom data structure
    VBlankInterrupt.is_Data = (APTR)&vbData;
    VBlankInterrupt.is_Node.ln_Pri = 0;  // set standard priority
    VBlankInterrupt.is_Node.ln_Name = "Some random name";

    // Add the VBlank interrupt server
    AddIntServer(INTB_VERTB, &VBlankInterrupt);

    /* Main loop: We wait for 5 seconds */
    start = vbData.frameCount;
    while (vbData.frameCount - start < 5 * VBLANKS_PER_SECOND) {
        /* Do your things: rendering, wait, read input,...*/
    }

    // Remove the interrupt server
    RemIntServer(INTB_VERTB, &VBlankInterrupt);

    // Finally, print the results
    printf("Frames: %lu, seconds: %lu\n",
           vbData.frameCount, vbData.seconds);

    return 0;
}
