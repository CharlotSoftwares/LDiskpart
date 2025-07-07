/*
 * PROJECT:         Linux DiskPart (conceptual)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/format.c
 * PURPOSE:         Formats partitions (Linux adaptation).
 * PROGRAMMERS:     Adapted by Radiump
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int BOOL;
#define TRUE 1
#define FALSE 0

static void usage(const char *progname)
{
    printf("Usage: %s <device> <filesystem_type>\n", progname);
    printf("Example: %s /dev/sda1 ext4\n", progname);
}

BOOL format_main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argv[0]);
        return FALSE;
    }

    const char *device = argv[1];
    const char *fs_type = argv[2];

    // Basic sanity check on device name
    if (strncmp(device, "/dev/", 5) != 0)
    {
        fprintf(stderr, "Error: Invalid device name '%s'. Must start with /dev/\n", device);
        return FALSE;
    }

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "mkfs -t %s %s", fs_type, device);

    printf("Formatting device %s with filesystem %s...\n", device, fs_type);
    int ret = system(cmd);
    if (ret != 0)
    {
        fprintf(stderr, "Format command failed with exit code %d\n", ret);
        return FALSE;
    }

    printf("Format completed successfully.\n");
    return TRUE;
}

// For testing as standalone program
#ifdef TEST_FORMAT_MAIN
int main(int argc, char **argv)
{
    return format_main(argc, argv) ? 0 : 1;
}
#endif
