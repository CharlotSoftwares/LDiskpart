/*
 * PROJECT:         Linux DiskPart (adapted from ReactOS DiskPart)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            select.c
 * PURPOSE:         Manage partitions interactively on Linux
 * PROGRAMMERS:     Adapted by Radiump for Linux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

// Example structs for disks, partitions, and volumes
typedef struct DiskEntry {
    int DiskNumber;
    struct DiskEntry* next;
    struct PartitionEntry* primaryPartitions;
    struct PartitionEntry* logicalPartitions;
} DiskEntry;

typedef struct PartitionEntry {
    int PartitionType;
    int PartitionNumber;
    struct PartitionEntry* next;
} PartitionEntry;

typedef struct VolumeEntry {
    int VolumeNumber;
    struct VolumeEntry* next;
} VolumeEntry;

// Globals simulating current selection and lists
DiskEntry* DiskListHead = NULL;
DiskEntry* CurrentDisk = NULL;

PartitionEntry* CurrentPartition = NULL;

VolumeEntry* VolumeListHead = NULL;
VolumeEntry* CurrentVolume = NULL;

// Utility: check if string is decimal number
int IsDecString(const char* str)
{
    if (!str || *str == '\0')
        return 0;
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
            return 0;
        str++;
    }
    return 1;
}

// Print functions (simulate ConResPrintf)
void PrintError(const char* msg)
{
    fprintf(stderr, "%s\n", msg);
}

void PrintInfo(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

/* FUNCTIONS */

int SelectDisk(int argc, char* argv[])
{
    DiskEntry* entry;
    int ulValue;

    if (argc > 3)
    {
        PrintError("Invalid number of arguments.");
        return 1;
    }

    if (argc == 2)
    {
        if (CurrentDisk == NULL)
            PrintInfo("No disk is currently selected.");
        else
            PrintInfo("Disk %d is currently selected.", CurrentDisk->DiskNumber);
        return 0;
    }

    if (strcasecmp(argv[2], "system") == 0)
    {
        if (DiskListHead == NULL)
        {
            PrintError("No disks found.");
            return 1;
        }
        CurrentDisk = DiskListHead;
        CurrentPartition = NULL;
        PrintInfo("Disk %d selected.", CurrentDisk->DiskNumber);
        return 0;
    }
    else if (strcasecmp(argv[2], "next") == 0)
    {
        if (CurrentDisk == NULL)
        {
            CurrentPartition = NULL;
            PrintError("Disk enumeration has not been started.");
            return 1;
        }

        if (CurrentDisk->next == NULL)
        {
            CurrentDisk = NULL;
            CurrentPartition = NULL;
            PrintError("Disk enumeration finished.");
            return 1;
        }

        CurrentDisk = CurrentDisk->next;
        CurrentPartition = NULL;
        PrintInfo("Disk %d selected.", CurrentDisk->DiskNumber);
        return 0;
    }
    else if (IsDecString(argv[2]))
    {
        ulValue = strtol(argv[2], NULL, 10);
        if (ulValue == 0 && errno == ERANGE)
        {
            PrintError("Invalid disk number.");
            return 1;
        }

        entry = DiskListHead;
        while (entry != NULL)
        {
            if (entry->DiskNumber == ulValue)
            {
                CurrentDisk = entry;
                CurrentPartition = NULL;
                PrintInfo("Disk %d selected.", CurrentDisk->DiskNumber);
                return 0;
            }
            entry = entry->next;
        }
    }
    else
    {
        PrintError("Invalid argument.");
        return 1;
    }

    PrintError("Disk not found.");
    return 1;
}

int SelectPartition(int argc, char* argv[])
{
    PartitionEntry* entry;
    int ulValue;
    int partNumber = 1;

    if (argc > 3)
    {
        PrintError("Invalid number of arguments.");
        return 1;
    }

    if (CurrentDisk == NULL)
    {
        PrintInfo("No disk selected.");
        return 1;
    }

    if (argc == 2)
    {
        if (CurrentPartition == NULL)
            PrintInfo("No partition is currently selected.");
        else
            PrintInfo("Partition %d is currently selected.", CurrentPartition->PartitionNumber);
        return 0;
    }

    if (!IsDecString(argv[2]))
    {
        PrintError("Invalid argument: partition number must be numeric.");
        return 1;
    }

    ulValue = strtol(argv[2], NULL, 10);
    if (ulValue == 0 && errno == ERANGE)
    {
        PrintError("Invalid partition number.");
        return 1;
    }

    // Search primary partitions
    entry = CurrentDisk->primaryPartitions;
    while (entry != NULL)
    {
        if (entry->PartitionType != 0)
        {
            if (partNumber == ulValue)
            {
                CurrentPartition = entry;
                PrintInfo("Partition %d selected.", partNumber);
                return 0;
            }
            partNumber++;
        }
        entry = entry->next;
    }

    // Search logical partitions
    entry = CurrentDisk->logicalPartitions;
    while (entry != NULL)
    {
        if (entry->PartitionType != 0)
        {
            if (partNumber == ulValue)
            {
                CurrentPartition = entry;
                PrintInfo("Partition %d selected.", partNumber);
                return 0;
            }
            partNumber++;
        }
        entry = entry->next;
    }

    PrintError("Partition not found.");
    return 1;
}

int SelectVolume(int argc, char* argv[])
{
    VolumeEntry* entry;
    int ulValue;

    if (argc > 3)
    {
        PrintError("Invalid number of arguments.");
        return 1;
    }

    if (argc == 2)
    {
        if (CurrentVolume == NULL)
            PrintInfo("No volume is currently selected.");
        else
            PrintInfo("Volume %d is currently selected.", CurrentVolume->VolumeNumber);
        return 0;
    }

    if (!IsDecString(argv[2]))
    {
        PrintError("Invalid argument: volume number must be numeric.");
        return 1;
    }

    ulValue = strtol(argv[2], NULL, 10);
    if (ulValue == 0 && errno == ERANGE)
    {
        PrintError("Invalid volume number.");
        return 1;
    }

    entry = VolumeListHead;
    while (entry != NULL)
    {
        if (entry->VolumeNumber == ulValue)
        {
            CurrentVolume = entry;
            PrintInfo("Volume %d selected.", CurrentVolume->VolumeNumber);
            return 0;
        }
        entry = entry->next;
    }

    PrintError("Volume not found.");
    return 1;
}
