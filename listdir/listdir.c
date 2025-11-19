/*
 * listdir.c - Verzeichnisinhalt auflisten
 *
 * Dieses Programm listet den Inhalt eines Verzeichnisses auf AmigaOS auf.
 * Verwendung: listdir
 */

#include <dos/dos.h>
#include <dos/exall.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <string.h>
#include <stdio.h>
#include <exec/memory.h>

#define BUFFERSIZE 512
#define DIR_PATH "example"

int main(int argc, char** argv) {
    BPTR lock;
    struct ExAllControl* exAllControl;
    struct ExAllData* exAllData;
    APTR buffer;
    LONG batch;
    LONG totalEntries = 0;

    // Lock the directory
    lock = Lock(DIR_PATH, ACCESS_READ);
    if (!lock) {
        printf("Could not open '%s'\n", DIR_PATH);
        return 20;
    }

    // Allocate buffer for ExAll()
    buffer = AllocVec(BUFFERSIZE, MEMF_CLEAR);
    if (!buffer) {
        UnLock(lock);
        printf("Error: Not enough memory\n");
        return 20;
    }

    // Allocate ExAllControl structure
    exAllControl = (struct ExAllControl*)AllocDosObject(DOS_EXALLCONTROL, NULL);
    if (!exAllControl) {
        FreeVec(buffer);
        UnLock(lock);
        printf("Error: Could not allocate ExAllControl\n");
        return 20;
    }

    // Reset resume cursor
    exAllControl->eac_LastKey = 0;

    printf("\nContents of '%s':\n", DIR_PATH);
    printf("%-30s %10s %s\n", "Name", "Size", "Type");
    printf("----------------------------------------------------------------\n");

    // Iterate through directory. One batch at a time
    batch = 1;
    while (batch) {
        batch = ExAll(lock, (struct ExAllData*)buffer, BUFFERSIZE,
                      ED_TYPE, exAllControl);

        if (!batch && IoErr() != ERROR_NO_MORE_ENTRIES) {
            printf("Error reading directory\n");
            break;
        }

        // Check if more entries are available
        if (exAllControl->eac_Entries == 0) {
            continue;
        }

        // Update total entries count
        totalEntries += exAllControl->eac_Entries;

        exAllData = (struct ExAllData*)buffer;

        while (exAllData) {
            char typestr[10];

            // Determine type
            if (exAllData->ed_Type > 0) {
                strcpy(typestr, "DIR");
            } else {
                strcpy(typestr, "File");
            }

            printf("%-30s %10ld %s\n",
                   exAllData->ed_Name,
                   exAllData->ed_Size,
                   typestr);

            exAllData = exAllData->ed_Next;
        }
    }

    printf("----------------------------------------------------------------\n");
    printf("Total: %ld entries\n", totalEntries);

    // Cleanup
    FreeDosObject(DOS_EXALLCONTROL, exAllControl);
    FreeVec(buffer);
    UnLock(lock);

    return 0;
}
