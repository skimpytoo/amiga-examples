/* Minimal example to play a ProTracker MOD with ptplayer on Amiga.
 * Steps:
 * 1) Open lowlevel.library (keyboard polling).
 * 2) Load a MOD into chip RAM (audio DMA needs chip memory).
 * 3) Detect PAL/NTSC and init ptplayer.
 * 4) Poll ESC and keep the system ticking with WaitTOF().
 * 5) Stop playback and clean up.
 */
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>
#include <clib/lowlevel_protos.h>
#include <hardware/custom.h>
#include <graphics/gfxbase.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>

#include <stdio.h>

#include "SDI/SDI_compiler.h"
#include "ptplayer/ptplayer.h"

#define custom (*(volatile struct Custom*)0xDFF000)
#define ESC_KEY 69

// Filename of the MOD to play (place file next to the executable)
#define MOD_NAME "BEAT_DANCE.MOD"

void cleanUp(void);

extern struct GfxBase* GfxBase;
UBYTE* modData = NULL;
int modSize = 0;
struct Library* LowLevelBase = NULL;
FILE* fp = NULL;

int main(int argc, char** argv) {
    BOOL is_pal;
    void* p_samples = NULL;
    UBYTE start_pos = 0;

    // Open lowlevel library to access keyboard functions
    if (!(LowLevelBase = OpenLibrary("lowlevel.library", 0L))) {
        printf("Couldn't open lowlevel.library\n");
        return RETURN_FAIL;
    }

    // Load the MOD file into chip memory (required for Paula DMA)
    fp = fopen(MOD_NAME, "rb");
    if (!fp) {
        cleanUp();
        printf("Could not open MOD file\n");
        return RETURN_FAIL;
    }

    fseek(fp, 0L, SEEK_END);
    modSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);  // Rewind to start

    printf("MOD file size: %d bytes\n", modSize);

    modData = (UBYTE*)AllocMem(modSize, MEMF_CHIP | MEMF_CLEAR);
    if (!modData) {
        printf("Could not allocate chip memory for MOD data\n");
        cleanUp();
        return RETURN_FAIL;
    }

    fread(modData, sizeof(UBYTE), modSize, fp);
    fclose(fp);

    // Detect PAL/NTSC to let ptplayer pick the right CIA timing
    is_pal = (((struct GfxBase*)GfxBase)->DisplayFlags & PAL) == PAL;

    // Initialize ptplayer and start playback
    mt_install_cia((struct Custom*)&custom, NULL, is_pal);
    mt_init((struct Custom*)&custom, modData, p_samples, start_pos);
    mt_Enable = 1;

    puts("Playing MOD... Press ESC to stop");

    // Main loop: poll ESC and wait for next frame to keep timing stable
    while (GetKey() != ESC_KEY) {
        WaitTOF();
    }

    cleanUp();
    return 0;
}

void cleanUp() {
    printf("Stopping playback and cleaning up...\n");
    if (mt_Enable == 1) {
        mt_Enable = 0;
        // Stop ptplayer first, then release CIA hooks
        mt_end((struct Custom*)&custom);
        mt_remove_cia((struct Custom*)&custom);
    }
    if (modData != NULL) {
        FreeMem(modData, modSize);
    }
    if (LowLevelBase != NULL) {
        CloseLibrary(LowLevelBase);
    }
    if (fp != NULL) {
        fclose(fp);
    }
}
