#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE (1024 * 1024) // 1 MB

typedef struct Disk {
    const char *device_path;
    off_t size_bytes;
    unsigned int sector_size;
} Disk;

// Global current disk pointer (should be initialized before calling clean_main)
Disk *CurrentDisk = NULL;

// Helper: get device size
off_t get_device_size(const char *device_path) {
    int fd = open(device_path, O_RDONLY);
    if (fd < 0) return -1;
    off_t size = lseek(fd, 0, SEEK_END);
    close(fd);
    return size;
}

int clean_main(int argc, char **argv) {
    bool wipe_all = false;

    if (CurrentDisk == NULL) {
        fprintf(stderr, "No disk selected. Please select a disk first.\n");
        return 1;
    }

    // Protect boot disk example: on Linux, usually device names or root disk detection can be used
    // For simplicity, we skip this check here

    // Parse arguments for "all"
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "all") == 0) {
            wipe_all = true;
            break;
        }
    }

    int fd = open(CurrentDisk->device_path, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device %s: %s\n", CurrentDisk->device_path, strerror(errno));
        return 1;
    }

    // Allocate 1MB zero buffer
    char *zero_buf = calloc(1, BUFFER_SIZE);
    if (!zero_buf) {
        fprintf(stderr, "Failed to allocate memory for zero buffer.\n");
        close(fd);
        return 1;
    }

    off_t total_size = CurrentDisk->size_bytes;
    if (total_size <= 0) {
        // Try to get device size if not known
        total_size = get_device_size(CurrentDisk->device_path);
        if (total_size <= 0) {
            fprintf(stderr, "Failed to determine device size.\n");
            free(zero_buf);
            close(fd);
            return 1;
        }
    }

    if (wipe_all) {
        off_t written = 0;
        while (written < total_size) {
            ssize_t to_write = BUFFER_SIZE;
            if (total_size - written < BUFFER_SIZE) {
                to_write = total_size - written;
            }
            ssize_t res = write(fd, zero_buf, to_write);
            if (res < 0) {
                fprintf(stderr, "Failed to write zeros at offset %lld: %s\n",
                        (long long)written, strerror(errno));
                free(zero_buf);
                close(fd);
                return 1;
            }
            written += res;
        }
    } else {
        // Wipe first MB
        if (write(fd, zero_buf, BUFFER_SIZE) != BUFFER_SIZE) {
            fprintf(stderr, "Failed to write zeros to start of disk: %s\n", strerror(errno));
            free(zero_buf);
            close(fd);
            return 1;
        }

        // Wipe last MB
        if (lseek(fd, total_size - BUFFER_SIZE, SEEK_SET) < 0) {
            fprintf(stderr, "Failed to seek to end of disk: %s\n", strerror(errno));
            free(zero_buf);
            close(fd);
            return 1;
        }
        if (write(fd, zero_buf, BUFFER_SIZE) != BUFFER_SIZE) {
            fprintf(stderr, "Failed to write zeros to end of disk: %s\n", strerror(errno));
            free(zero_buf);
            close(fd);
            return 1;
        }
    }

    printf("Disk cleaned successfully.\n");

    free(zero_buf);
    close(fd);
    return 0;
}

