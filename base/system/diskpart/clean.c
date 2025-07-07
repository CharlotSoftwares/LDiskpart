/*
 * PROJECT:         Linux DiskPart
 * LICENSE:         GPL
 * FILE:            base/system/diskpart/clean.c
 * PURPOSE:         Manages all the partitions of the OS in an interactive way.
 * PROGRAMMERS:     Lee Schroeder, Radiump
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define SECTOR_SIZE 512
#define MB (1024 * 1024)

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <device> [all]\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/sdx all\n", argv[0]);
        return 1;
    }

    const char *device_path = argv[1];
    int wipe_all = 0;

    if (argc > 2 && strcmp(argv[2], "all") == 0) {
        wipe_all = 1;
    }

    printf("Opening device: %s\n", device_path);
    int fd = open(device_path, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Get device size
    off_t device_size = lseek(fd, 0, SEEK_END);
    if (device_size == -1) {
        perror("Failed to get device size");
        close(fd);
        return 1;
    }
    printf("Device size: %lld bytes (%.2f GB)\n", (long long)device_size, device_size / (1024.0 * 1024.0 * 1024.0));

    // Allocate 1MB zero buffer
    void *zero_buffer = calloc(1, MB);
    if (!zero_buffer) {
        fprintf(stderr, "Failed to allocate zero buffer\n");
        close(fd);
        return 1;
    }

    ssize_t written;
    off_t offset;

    if (wipe_all) {
        printf("Wiping entire device with zeros...\n");

        off_t total_written = 0;
        off_t to_write = device_size;
        while (to_write > 0) {
            ssize_t chunk = (to_write > MB) ? MB : to_write;

            offset = lseek(fd, total_written, SEEK_SET);
            if (offset == -1) {
                perror("Failed to seek");
                break;
            }

            written = write(fd, zero_buffer, chunk);
            if (written == -1) {
                perror("Failed to write zeros");
                break;
            }

            total_written += written;
            to_write -= written;

            printf("\rProgress: %.2f%%", (total_written * 100.0) / device_size);
            fflush(stdout);
        }
        printf("\n");
    }
    else {
        printf("Wiping first MB...\n");
        offset = lseek(fd, 0, SEEK_SET);
        if (offset == -1 || write(fd, zero_buffer, MB) == -1) {
            perror("Failed to wipe first MB");
            free(zero_buffer);
            close(fd);
            return 1;
        }

        printf("Wiping last MB...\n");
        offset = lseek(fd, device_size - MB, SEEK_SET);
        if (offset == -1 || write(fd, zero_buffer, MB) == -1) {
            perror("Failed to wipe last MB");
            free(zero_buffer);
            close(fd);
            return 1;
        }
    }

    printf("Disk clean operation completed successfully.\n");

    free(zero_buffer);
    close(fd);

    return 0;
}
