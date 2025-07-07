/*
 * PROJECT:         Linux DiskPart-like tool (ported)
 * LICENSE:         GPL - See COPYING
 * FILE:            detail.c
 * PURPOSE:         Show details about disks, partitions, and volumes
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Dummy linked list struct
typedef struct ListEntry {
    struct ListEntry *next;
    struct ListEntry *prev;
} ListEntry;

// Disk, Partition, Volume structures simplified
typedef struct {
    uint32_t DiskNumber;
    uint64_t BytesPerSector;
} DiskEntry;

typedef struct {
    uint64_t QuadPart;
} LargeInteger;

typedef struct {
    uint32_t NumberOfDiskExtents;
    struct {
        uint32_t DiskNumber;
        LargeInteger StartingOffset;
        LargeInteger ExtentLength;
    } *Extents;
} Extents;

typedef struct {
    Extents *pExtents;
    ListEntry ListEntry;
} VolumeEntry;

typedef struct {
    uint32_t PartitionNumber;
    uint32_t PartitionType;
    bool BootIndicator;
    LargeInteger StartSector;
    LargeInteger SectorCount;
    DiskEntry *DiskEntry;
    ListEntry ListEntry;
} PartEntry;

// Global heads of lists (dummy initialization)
ListEntry VolumeListHead = { &VolumeListHead, &VolumeListHead };
ListEntry DiskListHead = { &DiskListHead, &DiskListHead };

// Current selections (to simulate state)
DiskEntry *CurrentDisk = NULL;
PartEntry *CurrentPartition = NULL;
VolumeEntry *CurrentVolume = NULL;

// Helper macro for linked list traversal
#define LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

// Dummy functions to simulate printing and linked list behavior

void PrintVolume(VolumeEntry *vol) {
    printf("Volume info: %p\n", (void*)vol);
}

void PrintDisk(DiskEntry *disk) {
    printf("Disk %u info\n", disk->DiskNumber);
}

bool IsDiskInVolume(VolumeEntry *vol, DiskEntry *disk) {
    if (!vol || !vol->pExtents || !disk)
        return false;

    for (uint32_t i = 0; i < vol->pExtents->NumberOfDiskExtents; i++) {
        if (vol->pExtents->Extents[i].DiskNumber == disk->DiskNumber)
            return true;
    }
    return false;
}

bool IsPartitionInVolume(VolumeEntry *vol, PartEntry *part) {
    if (!vol || !vol->pExtents || !part || !part->DiskEntry)
        return false;

    for (uint32_t i = 0; i < vol->pExtents->NumberOfDiskExtents; i++) {
        if (vol->pExtents->Extents[i].DiskNumber == part->DiskEntry->DiskNumber) {
            if (vol->pExtents->Extents[i].StartingOffset.QuadPart == part->StartSector.QuadPart * part->DiskEntry->BytesPerSector &&
                vol->pExtents->Extents[i].ExtentLength.QuadPart == part->SectorCount.QuadPart * part->DiskEntry->BytesPerSector)
                return true;
        }
    }
    return false;
}

// DetailDisk equivalent
int DetailDisk(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }
    if (!CurrentDisk) {
        printf("No disk selected\n");
        return 1;
    }

    printf("\nDisk Signature: %u\n", CurrentDisk->DiskNumber);  // Replace with real sig
    printf("PathId: N/A\n");    // No equivalent in Linux example
    printf("TargetId: N/A\n");
    printf("Lun: N/A\n");

    ListEntry *entry;
    bool printedHeader = false;

    LIST_FOR_EACH(entry, &VolumeListHead) {
        VolumeEntry *vol = (VolumeEntry *)((char *)entry - offsetof(VolumeEntry, ListEntry));
        if (IsDiskInVolume(vol, CurrentDisk)) {
            if (!printedHeader) {
                printf("\nVolumes:\n");
                printedHeader = true;
            }
            PrintVolume(vol);
        }
    }

    printf("\n");
    return 0;
}

// DetailPartition equivalent
int DetailPartition(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }
    if (!CurrentDisk) {
        printf("No disk selected\n");
        return 1;
    }
    if (!CurrentPartition) {
        printf("No partition selected\n");
        return 1;
    }

    uint64_t partOffset = CurrentPartition->StartSector.QuadPart * CurrentDisk->BytesPerSector;

    printf("\nPartition Number: %u\n", CurrentPartition->PartitionNumber);
    printf("Partition Type: %u\n", CurrentPartition->PartitionType);
    printf("Hidden: N/A\n");
    printf("Active: %s\n", CurrentPartition->BootIndicator ? "Yes" : "No");
    printf("Offset: %lu\n", partOffset);

    ListEntry *entry;
    bool volumeFound = false;
    bool printedHeader = false;

    LIST_FOR_EACH(entry, &VolumeListHead) {
        VolumeEntry *vol = (VolumeEntry *)((char *)entry - offsetof(VolumeEntry, ListEntry));
        if (IsPartitionInVolume(vol, CurrentPartition)) {
            if (!printedHeader) {
                printf("\nVolumes:\n");
                printedHeader = true;
            }
            PrintVolume(vol);
            volumeFound = true;
        }
    }

    if (!volumeFound)
        printf("No volume found\n");

    printf("\n");
    return 0;
}

// DetailVolume equivalent
int DetailVolume(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "Invalid arguments\n");
        return 1;
    }

    if (!CurrentVolume) {
        printf("No volume selected\n");
        return 1;
    }

    ListEntry *entry;
    bool diskFound = false;
    bool printedHeader = false;

    LIST_FOR_EACH(entry, &DiskListHead) {
        DiskEntry *disk = (DiskEntry *)((char *)entry - offsetof(DiskEntry, ListEntry));
        if (IsDiskInVolume(CurrentVolume, disk)) {
            if (!printedHeader) {
                printf("\nDisks:\n");
                printedHeader = true;
            }
            PrintDisk(disk);
            diskFound = true;
        }
    }

    if (!diskFound)
        printf("No disks found\n");

    printf("\n");
    return 0;
}

