#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/fs.h>  // For BLKGETSIZE64

#define MBR_SIZE 512
#define PARTITION_TABLE_OFFSET 446
#define PARTITION_ENTRY_SIZE 16
#define MAX_PARTITIONS 4

struct mbr_partition_entry {
    unsigned char boot_flag;
    unsigned char chs_start[3];
    unsigned char type;
    unsigned char chs_end[3];
    unsigned int lba_start;
    unsigned int num_sectors;
} __attribute__((packed));

struct mbr {
    unsigned char boot_code[446];
    struct mbr_partition_entry partitions[MAX_PARTITIONS];
    unsigned short signature;
} __attribute__((packed));

int delete_partition(const char *device, int partition_index)
{
    if (partition_index < 1 || partition_index > MAX_PARTITIONS) {
        fprintf(stderr, "Partition index must be between 1 and 4\n");
        return -1;
    }

    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("Open device");
        return -1;
    }

    struct mbr mbr_data;

    // Read MBR
    ssize_t bytes_read = pread(fd, &mbr_data, MBR_SIZE, 0);
    if (bytes_read != MBR_SIZE) {
        perror("Read MBR");
        close(fd);
        return -1;
    }

    // Zero out the partition entry
    memset(&mbr_data.partitions[partition_index -1], 0, sizeof(struct mbr_partition_entry));

    // Write MBR back
    ssize_t bytes_written = pwrite(fd, &mbr_data, MBR_SIZE, 0);
    if (bytes_written != MBR_SIZE) {
        perror("Write MBR");
        close(fd);
        return -1;
    }

    // Tell kernel to reread partition table
    if (ioctl(fd, BLKRRPART) < 0) {
        perror("ioctl BLKRRPART");
        // Not fatal, just warn
    }

    close(fd);

    printf("Partition %d deleted successfully on device %s\n", partition_index, device);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <device> <partition_number>\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/sda 2\n", argv[0]);
        return 1;
    }

    const char *device = argv[1];
    int partition_index = atoi(argv[2]);

    if (delete_partition(device, partition_index) < 0) {
        fprintf(stderr, "Failed to delete partition\n");
        return 1;
    }

    return 0;
}
