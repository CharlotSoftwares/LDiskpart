/*
 * PROJECT:         Linux DiskPart GPT Support (conceptual)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/gpt.c
 * PURPOSE:         Manages GPT partitions (Linux adaptation).
 * PROGRAMMERS:     Adapted by Anonymous
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parted/parted.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0

static void usage(const char *progname)
{
    printf("Usage: %s <device>\n", progname);
    printf("Example: %s /dev/sdx\n", progname);
}

BOOL gpt_main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(argv[0]);
        return FALSE;
    }

    const char *device_path = argv[1];
    PedDevice *device = NULL;
    PedDisk *disk = NULL;

    ped_device_probe_all();

    device = ped_device_get(device_path);
    if (!device)
    {
        fprintf(stderr, "Error: Cannot open device %s\n", device_path);
        return FALSE;
    }

    disk = ped_disk_new(device);
    if (!disk)
    {
        fprintf(stderr, "No partition table found on %s\n", device_path);
        ped_device_destroy(device);
        return FALSE;
    }

    if (disk->type && strcmp(disk->type->name, "gpt") == 0)
    {
        printf("Device %s uses GPT partition table.\n", device_path);
    }
    else
    {
        printf("Device %s does not use GPT partition table.\n", device_path);
    }

    ped_disk_destroy(disk);
    ped_device_destroy(device);

    return TRUE;
}

// For standalone test
#ifdef TEST_GPT_MAIN
int main(int argc, char **argv)
{
    return gpt_main(argc, argv) ? 0 : 1;
}
#endif
