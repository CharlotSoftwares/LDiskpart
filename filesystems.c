/*
 * PROJECT:         Linux DiskPart (conceptual)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/filesystems.c
 * PURPOSE:         Show filesystem info on Linux.
 * PROGRAMMERS:     Adapted by Radiump
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statfs.h>
#include <mntent.h>
#include <errno.h>

#define MAX_MOUNT_LEN 256

// Show filesystem info for the device mounted at CurrentVolume->DeviceName
static int ShowFileSystemInfo(const char *device)
{
    FILE *mntfp;
    struct mntent *mnt;
    struct statfs fsinfo;
    int found = 0;

    // Open mount table
    mntfp = setmntent("/proc/mounts", "r");
    if (mntfp == NULL)
    {
        perror("setmntent");
        return 0;
    }

    // Search for device in mounts
    while ((mnt = getmntent(mntfp)) != NULL)
    {
        if (strcmp(mnt->mnt_fsname, device) == 0)
        {
            found = 1;
            printf("Filesystem type: %s\n", mnt->mnt_type);

            if (statfs(mnt->mnt_dir, &fsinfo) == 0)
            {
                unsigned long block_size = fsinfo.f_bsize;
                unsigned long cluster_size = block_size * fsinfo.f_bfree; // Approximate cluster size

                printf("Block size: %lu bytes\n", block_size);
                printf("Free blocks: %lu\n", (unsigned long)fsinfo.f_bfree);
            }
            else
            {
                perror("statfs");
            }

            break;
        }
    }

    endmntent(mntfp);

    if (!found)
    {
        printf("Device %s is not currently mounted or not found in /proc/mounts\n", device);
        return 0;
    }

    return 1;
}

static void ShowInstalledFileSystems(void)
{
    FILE *fstabfp;
    char line[512];

    printf("Supported filesystems (from /proc/filesystems):\n");

    fstabfp = fopen("/proc/filesystems", "r");
    if (!fstabfp)
    {
        perror("fopen /proc/filesystems");
        return;
    }

    while (fgets(line, sizeof(line), fstabfp))
    {
        // /proc/filesystems has lines like: nodev   sysfs
        // We'll print only filesystem names (second column)
        char *fs = strchr(line, '\t');
        if (fs)
            printf("  %s", fs + 1);
        else
            printf("  %s", line);
    }

    fclose(fstabfp);
    printf("\n");
}

// Simulated CurrentVolume struct
typedef struct {
    char DeviceName[MAX_MOUNT_LEN];
} VOLENTRY;

VOLENTRY *CurrentVolume = NULL;

int filesystems_main(int argc, char **argv)
{
    if (CurrentVolume == NULL)
    {
        printf("No volume selected. Use select volume first.\n");
        return 1;
    }

    printf("\n");

    if (ShowFileSystemInfo(CurrentVolume->DeviceName))
    {
        ShowInstalledFileSystems();
    }

    return 0;
}

int main(int argc, char **argv)
{
    // Example usage
    VOLENTRY vol;
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <device>\n", argv[0]);
        return 1;
    }

    strncpy(vol.DeviceName, argv[1], MAX_MOUNT_LEN - 1);
    vol.DeviceName[MAX_MOUNT_LEN - 1] = '\0';
    CurrentVolume = &vol;

    return filesystems_main(argc, argv);
}

