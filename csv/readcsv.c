#include <exec/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>

// Maximum line length in csv file. Required for the read buffer.
#define MAX_LINE_LENGTH 150

// Struct to store a line of the csv file
typedef struct TestStruct {
    char fileName[50];  // filename
    char name[20];      // name
    int x;              // x value
    int y;              // y value
};

// Container of the parsed result
typedef struct TestStructContainer {
    int length;                     //  Number of entries
    struct TestStruct *testStruct;  //  List of entries
};

/**
 * @brief Get the number Of lines in file.
 *
 * @param fp Filepointer.
 * @param skipHeader TRUE if the first line is a header and should not get parsed.
 * @return int number of files.
 */
int getNumberOfLines(FILE *fp, BOOL skipHeader) {
    int length = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        length++;
    }
    if (skipHeader) length--;
    rewind(fp);
    return length;
}

/**
 * @brief Read csv file and store content in an object
 *
 * @param filename csv file name.
 * @param skipHeader TRUE if the first line is a header and should not get parsed.
 * @return struct TestStructContainer* Parsed object. NULL in error case.
 */
struct TestStructContainer *readCsv(char *filename, BOOL skipHeader) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    int i = 0;
    int x = 0, y = 0;
    struct TestStructContainer *testStructContainer = NULL;

    printf("Try to open file: %s\n", filename);
    // Allocate memory for the container of the csv entries
    testStructContainer = (struct TestStructContainer *)AllocMem(sizeof(struct TestStructContainer), MEMF_ANY);
    if (testStructContainer == NULL) {
        return NULL;
    }

    fp = fopen(filename, "r");
    if (fp != NULL) {
        printf("Reading file:");
        testStructContainer->length = getNumberOfLines(fp, TRUE);

        // Allocate memory for all csv entries
        testStructContainer->testStruct = (struct TestStruct *)AllocMem(testStructContainer->length * sizeof(struct TestStruct), MEMF_ANY | MEMF_CLEAR);

        if (skipHeader) {
            fgets(line, MAX_LINE_LENGTH, fp);
        }
        //  read each line
        while (fgets(line, MAX_LINE_LENGTH, fp)) {
            // parse file %[^;] - string, %d - number
            sscanf(line, "%[^;];%[^;];%d;%d", testStructContainer->testStruct[i].name, testStructContainer->testStruct[i].fileName, &x, &y);
            testStructContainer->testStruct[i].x = x;
            testStructContainer->testStruct[i].y = y;
            i++;
            printf("*");
        }
        fclose(fp);
        printf("\nFinished\n");
    } else {
        printf("Could not open %s\n", filename);
        FreeMem(testStructContainer, testStructContainer->length * sizeof(struct TestStructContainer));
    }
    return testStructContainer;
}

VOID main(int argc, char **argv) {
    int i;
    struct TestStructContainer *testStructContainer;

    testStructContainer = readCsv("testfile.csv", TRUE);

    if (testStructContainer != NULL) {
        printf("Entries: %d\n", testStructContainer->length);

        for (i = 0; i < testStructContainer->length; i++) {
            printf("Entry %d:\n Name:%s File:%s X:%d Y:%d\n", i + 1, testStructContainer->testStruct[i].name, testStructContainer->testStruct[i].fileName, testStructContainer->testStruct[i].x, testStructContainer->testStruct[i].y);
        }

        FreeMem(testStructContainer->testStruct, testStructContainer->length * sizeof(struct TestStruct));
        FreeMem(testStructContainer, sizeof(struct TestStructContainer));
    }
}