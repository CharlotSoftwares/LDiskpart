#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <parted/parted.h>

typedef struct Disk {
    PedDevice *device;
    PedDisk *disk;
} Disk;

typedef struct Partition {
    PedPartition *partition;
} Partition;

// Globals for current selections
Disk *CurrentDisk = NULL;
Partition *CurrentPartition = NULL;

// Initialize libparted device and disk
Disk *select_disk(const char *dev_path) {
    PedDevice *dev = ped_device_get(dev_path);
    if (!dev) {
        fprintf(stderr, "Error: Could not open device %s\n", dev_path);
        return NULL;
    }

    if (!ped_device_open(dev)) {
        fprintf(stderr, "Error: Could not open device %s\n", dev_path);
        ped_device_destroy(dev);
        return NULL;
    }

    PedDisk *disk = ped_disk_new(dev);
    if (!disk) {
        fprintf(stderr, "Error: Could not read partition table on %s\n", dev_path);
        ped_device_close(dev);
        ped_device_destroy(dev);
        return NULL;
    }

    Disk *d = malloc(sizeof(Disk));
    d->device = dev;
    d->disk = disk;
    return d;
}

// Free disk struct and close device
void free_disk(Disk *d) {
    if (d) {
        if (d->disk)
            ped_disk_destroy(d->disk);
        if (d->device) {
            ped_device_close(d->device);
            ped_device_destroy(d->device);
        }
        free(d);
    }
}

// Select a partition by number (1-based)
Partition *select_partition(Disk *disk, int part_num) {
    if (!disk || !disk->disk) return NULL;

    PedPartition *part = ped_disk_get_partition(disk->disk, part_num);
    if (!part) {
        fprintf(stderr, "Error: Partition %d not found.\n", part_num);
        return NULL;
    }

    Partition *p = malloc(sizeof(Partition));
    p->partition = part;
    return p;
}

void free_partition(Partition *p) {
    if (p) free(p);
}

// Mark the partition as active (boot flag) on MBR
bool set_partition_active(Disk *disk, Partition *part) {
    if (!disk || !disk->disk || !part || !part->partition) return false;

    // Clear boot flag on all partitions first
    PedPartition *p_iter;
    ped_disk_set_modified(disk->disk, true);

    for (p_iter = ped_disk_next_partition(disk->disk, NULL);
         p_iter != NULL;
         p_iter = ped_disk_next_partition(disk->disk, p_iter)) {
        if (p_iter->type == PED_PARTITION_NORMAL) {
            ped_partition_set_flag(p_iter, PED_PARTITION_BOOT, 0);
        }
    }

    // Set boot flag on selected partition
    ped_partition_set_flag(part->partition, PED_PARTITION_BOOT, 1);

    // Commit changes
    if (!ped_disk_commit(disk->disk)) {
        fprintf(stderr, "Error: Failed to commit changes to disk.\n");
        return false;
    }

    if (!ped_device_sync(disk->device)) {
        fprintf(stderr, "Error: Failed to sync device.\n");
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <disk_device_path> <partition_number>\n", argv[0]);
        printf("Example: %s /dev/sda 1\n", argv[0]);
        return 1;
    }

    const char *disk_path = argv[1];
    int part_num = atoi(argv[2]);
    if (part_num <= 0) {
        fprintf(stderr, "Invalid partition number.\n");
        return 1;
    }

    ped_device_probe_all();

    CurrentDisk = select_disk(disk_path);
    if (!CurrentDisk) {
        return 1;
    }

    CurrentPartition = select_partition(CurrentDisk, part_num);
    if (!CurrentPartition) {
        free_disk(CurrentDisk);
        return 1;
    }

    printf("Setting partition %d on %s as active (bootable)...\n", part_num, disk_path);

    if (set_partition_active(CurrentDisk, CurrentPartition)) {
        printf("Partition set as active successfully.\n");
    } else {
        printf("Failed to set partition as active.\n");
    }

    free_partition(CurrentPartition);
    free_disk(CurrentDisk);

    return 0;
}

