/*
 * PROJECT:         Linux DiskPart-like tool
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            rescan.c
 * PURPOSE:         Rescan and refresh partition and volume lists interactively.
 * PROGRAMMERS:     Radiump
 */

#include <stdio.h>

// Declare functions you would have implemented for managing partitions/volumes
void DestroyVolumeList(void);
void DestroyPartitionList(void);
void CreatePartitionList(void);
void CreateVolumeList(void);

int rescan_main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Rescan started...\n");

    DestroyVolumeList();
    DestroyPartitionList();
    CreatePartitionList();
    CreateVolumeList();

    printf("Rescan finished.\n");

    return 0; // Success
}
