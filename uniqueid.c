/*
 * PROJECT:         Linux DiskPart (adapted from ReactOS DiskPart)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            uniqueid.c
 * PURPOSE:         Manage disk unique IDs interactively on Linux
 * PROGRAMMERS:     Adapted by Radiump for Linux
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

// Dummy structs to simulate ReactOS DiskPart environment
typedef struct _DISK {
    unsigned long Signature;
    int Dirty;
    // ... other members as needed ...
} DISK;

DISK *CurrentDisk = NULL;

// Helpers
int IsHexString(const char *str)
{
    while (*str)
    {
        if (!isxdigit((unsigned char)*str))
            return 0;
        str++;
    }
    return 1;
}

int HasPrefix(const char *str, const char *prefix, const char **suffix)
{
    size_t len = strlen(prefix);
    if (strncmp(str, prefix, len) == 0)
    {
        *suffix = str + len;
        return 1;
    }
    return 0;
}

void UpdateDiskLayout(DISK *disk)
{
    // TODO: implement actual disk layout update logic
    printf("[DEBUG] Disk layout updated\n");
}

void WritePartitions(DISK *disk)
{
    // TODO: implement actual write partitions logic
    printf("[DEBUG] Partitions written to disk\n");
}

// The adapted UniqueIdDisk function
int UniqueIdDisk(int argc, char *argv[])
{
    const char *pszSuffix = NULL;
    unsigned long ulValue;

    if (CurrentDisk == NULL)
    {
        printf("No disk selected.\n");
        return 1;
    }

    if (argc == 2)
    {
        printf("\nDisk ID: %08lx\n\n", CurrentDisk->Signature);
        return 0;
    }

    if (argc != 3)
    {
        printf("Invalid arguments.\n");
        return 1;
    }

    if (!HasPrefix(argv[2], "ID=", &pszSuffix))
    {
        printf("Invalid arguments.\n");
        return 1;
    }

    if (pszSuffix == NULL || strlen(pszSuffix) != 8 || !IsHexString(pszSuffix))
    {
        printf("Invalid arguments.\n");
        return 1;
    }

    errno = 0;
    ulValue = strtoul(pszSuffix, NULL, 16);
    if ((ulValue == 0) && (errno == ERANGE))
    {
        printf("Invalid arguments.\n");
        return 1;
    }

    printf("New Signature: 0x%08lx\n", ulValue);
    CurrentDisk->Signature = ulValue;
    CurrentDisk->Dirty = 1;

    UpdateDiskLayout(CurrentDisk);
    WritePartitions(CurrentDisk);

    return 0;
}
