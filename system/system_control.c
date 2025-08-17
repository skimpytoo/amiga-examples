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

#define RUNS 3

/* ----- Own data structure to pass information to intervall ----- */
struct MyCustomData {
    ULONG interrupts;
};

struct MyCustomData vbData;
/* ----- Interrupt-Service-Routine ----- */
__saveds __interrupt BOOL MyVBlankISR(void) {
    struct MyCustomData *data = &vbData;
    data->interrupts++;
    return TRUE;
}

int main(void) {
    struct Library *LowLevelBase = NULL;
    APTR intHandle = NULL;
    ULONG count_no_overtake = 0, count_overtake = 0, failTag;

    if (!(LowLevelBase = OpenLibrary("lowlevel.library", 0L))) {
        printf("Couldn't open lowlevel.library\n");
        return RETURN_FAIL;
    }

    vbData.interrupts = 0;

    // Add the timer interrupt. Function to call and data
    intHandle = AddTimerInt(MyVBlankISR, (APTR)&vbData);

    printf("Start: Without system overtake\n");

    StartTimerInt(intHandle, 200000, TRUE);

    while (vbData.interrupts < RUNS) {
        count_no_overtake++;
    }

    // Stop the continuous timer interrupt
    StopTimerInt(intHandle);
    // Remove the interrupt
    RemTimerInt(intHandle);

    /* With overtaking system */
    vbData.interrupts = 0;
    printf("Start: With system overtake\n");
    // Add the timer interrupt. Function to call and data
    intHandle = AddTimerInt(MyVBlankISR, (APTR)&vbData);

    /*
     * Enable the system overtake.
     */
    failTag = SystemControl(SCON_TakeOverSys, TRUE, TAG_DONE);

    StartTimerInt(intHandle, 200000, TRUE);

    while (vbData.interrupts < RUNS) {
        count_overtake++;
    }

    // Stop the continuous timer interrupt
    StopTimerInt(intHandle);
    // Remove the interrupt
    RemTimerInt(intHandle);

    failTag = SystemControl(SCON_TakeOverSys, FALSE, TAG_DONE);
    //  Close the lowlevel.library
    if (LowLevelBase) {
        CloseLibrary(LowLevelBase);
    }
    // Finally, print the results
    printf("Counts no_system_overtake: %lu - Counts system_overtake: %lu Difference: %d\n",
           count_no_overtake, count_overtake, count_overtake - count_no_overtake);

    return 0;
}
