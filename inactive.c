/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/inactive.c
 * PURPOSE:         Manage disk partitions in an interactive way.
 * PROGRAMMERS:     Lee Schroeder (original)
 *                  Adapted for Linux by Radiump
 */

#include "diskpart.h"

#include <stdio.h>
#include <stdbool.h>

bool inactive_main(int argc, wchar_t **argv)
{
    if (CurrentDisk == NULL)
    {
        printf("No disk selected.\n");
        return true;
    }

    if (CurrentPartition == NULL)
    {
        printf("No partition selected.\n");
        return true;
    }

    if (!CurrentPartition->BootIndicator)
    {
        printf("Partition is already inactive.\n");
        return true;
    }

    // Clear boot indicator
    CurrentPartition->BootIndicator = false;
    CurrentDisk->Dirty = true;

    // Update disk layout and write changes back to disk
    UpdateDiskLayout(CurrentDisk);

    if (WritePartitions(CurrentDisk) == 0) // assume 0 == success in Linux version
    {
        printf("Partition marked as inactive successfully.\n");
    }
    else
    {
        printf("Failed to mark partition as inactive.\n");
    }

    return true;
}
