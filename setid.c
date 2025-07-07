/*
 * PROJECT:         Linux DiskPart (adapted from ReactOS DiskPart)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            setid.c
 * PURPOSE:         Manage partition ID (type) in an interactive way on Linux
 * PROGRAMMERS:     Adapted by CB for Linux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned char UCHAR;

typedef struct PartitionEntry {
    UCHAR PartitionType;
    // ... other members
} PartitionEntry;

typedef struct DiskEntry {
    int Dirty;
    // ... other members
} DiskEntry;

// Assume these globals exist
extern DiskEntry* CurrentDisk;
extern PartitionEntry* CurrentPartition;

// Stub functions for demo (replace with real implementations)
void UpdateDiskLayout(DiskEntry* disk) {
    // update disk layout
}
int WritePartitions(DiskEntry* disk) {
    // return 0 on success, non-zero on failure
    return 0;
}

int IsHexString(const char* str) {
    while (*str) {
        if (!isxdigit((unsigned char)*str))
            return 0;
        str++;
    }
    return 1;
}

int setid_main(int argc, char* argv[])
{
    UCHAR PartitionType = 0;
    int i;

    if (CurrentDisk == NULL)
    {
        fprintf(stdout, "No disk selected.\n");
        return 1;
    }

    if (CurrentPartition == NULL)
    {
        fprintf(stdout, "No partition selected.\n");
        return 1;
    }

    for (i = 1; i < argc; i++)
    {
        // Expect argument format "id=<hexbyte>"
        if (strncmp(argv[i], "id=", 3) == 0)
        {
            const char* idStr = argv[i] + 3;

            size_t len = strlen(idStr);
            if (len == 0)
            {
                fprintf(stderr, "Invalid argument: id= value missing.\n");
                return 1;
            }

            if (len > 2)
            {
                fprintf(stderr, "Invalid format: id= must be 1 or 2 hex digits.\n");
                return 1;
            }

            if (!IsHexString(idStr))
            {
                fprintf(stderr, "Invalid format: id= must be hex digits.\n");
                return 1;
            }

            PartitionType = (UCHAR)strtol(idStr, NULL, 16);
            if (PartitionType == 0)
            {
                fprintf(stderr, "Invalid partition type: 0 is not allowed.\n");
                return 1;
            }
        }
    }

    if (PartitionType == 0x42)
    {
        fprintf(stderr, "Partition type 0x42 is invalid.\n");
        return 1;
    }

    // Apply changes
    CurrentPartition->PartitionType = PartitionType;
    CurrentDisk->Dirty = 1;
    UpdateDiskLayout(CurrentDisk);

    if (WritePartitions(CurrentDisk) != 0)
    {
        fprintf(stdout, "Failed to write partition information.\n");
        return 1;
    }

    fprintf(stdout, "Partition ID set successfully.\n");
    return 0;
}
