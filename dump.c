#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

static void HexDump(const unsigned char *addr, int len)
{
    char buffer[17];
    int i;

    buffer[16] = '\0';

    for (i = 0; i < len; i++)
    {
        if ((i % 16) == 0)
            printf(" %04x ", i);

        printf(" %02x", addr[i]);

        if (!isprint(addr[i]))
            buffer[i % 16] = '.';
        else
            buffer[i % 16] = addr[i];

        if ((i % 16) == 15)
            printf("  %s\n", buffer);
    }

    // Print last line if not complete 16 bytes
    if ((len % 16) != 0)
    {
        int rem = 16 - (len % 16);
        for (i = 0; i < rem; i++)
            printf("   ");
        printf("  %s\n", buffer);
    }
}

int DumpDisk(const char *device, int64_t sector, size_t sector_size)
{
    int fd = open(device, O_RDONLY);
    if (fd < 0)
    {
        perror("Failed to open device");
        return -1;
    }

    unsigned char *buffer = malloc(sector_size);
    if (!buffer)
    {
        perror("Failed to allocate buffer");
        close(fd);
        return -1;
    }

    off_t offset = sector * sector_size;

    ssize_t bytes_read = pread(fd, buffer, sector_size, offset);
    if (bytes_read != (ssize_t)sector_size)
    {
        perror("Failed to read sector");
        free(buffer);
        close(fd);
        return -1;
    }

    HexDump(buffer, sector_size);

    free(buffer);
    close(fd);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <device> <sector_number> <sector_size>\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/sda 0 512\n", argv[0]);
        return 1;
    }

    const char *device = argv[1];
    int64_t sector = atoll(argv[2]);
    size_t sector_size = (size_t)atoi(argv[3]);

    if (sector < 0 || sector_size == 0)
    {
        fprintf(stderr, "Invalid sector number or sector size\n");
        return 1;
    }

    return DumpDisk(device, sector, sector_size);
}
