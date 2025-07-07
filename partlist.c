#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <errno.h>
#include <linux/hdreg.h>
#include <sys/types.h>
#include <sys/statvfs.h>

typedef struct _PARTENTRY {
    char device_path[256];   // e.g. "/dev/sda1"
    unsigned long long start_sector;
    unsigned long long sector_count;
    int partition_type;      // Partition type ID or code (e.g. Linux = 0x83)
    bool is_partitioned;
    bool logical_partition;
    int partition_number;
    int partition_index;
    bool boot_indicator;
    bool new_partition;
    bool dirty;
    struct _DISKENTRY *disk_entry;
    struct _PARTENTRY *next;
} PARTENTRY;

typedef struct _DISKENTRY {
    char device_path[256];  // e.g. "/dev/sda"
    unsigned long long bytes_per_sector;
    unsigned int sector_alignment;
    bool dirty;
    PARTENTRY *primary_partitions;
    PARTENTRY *logical_partitions;
    int partition_count;
} DISKENTRY;


// -- Dismount volume by unmounting the device
int DismountVolume(PARTENTRY *part_entry)
{
    if (!part_entry || !part_entry->is_partitioned)
        return 0;  // Success, nothing to do

    // Assuming device_path points to e.g. "/dev/sda1"
    // We try to umount the mountpoint if it is mounted

    // Get mountpoint by reading /proc/mounts or /etc/mtab (simplified)
    FILE *mounts = fopen("/proc/mounts", "r");
    if (!mounts) {
        perror("Failed to open /proc/mounts");
        return -1;
    }

    char line[1024];
    char device[256];
    char mountpoint[256];
    int found = 0;

    while (fgets(line, sizeof(line), mounts)) {
        if (sscanf(line, "%255s %255s", device, mountpoint) == 2) {
            if (strcmp(device, part_entry->device_path) == 0) {
                found = 1;
                break;
            }
        }
    }
    fclose(mounts);

    if (!found) {
        // Not mounted, nothing to do
        return 0;
    }

    // Attempt to unmount
    if (umount(mountpoint) != 0) {
        perror("Failed to unmount volume");
        return -1;
    }

    return 0;
}

// -- Update disk layout (simplified for Linux)
// Linux partition tables are manipulated with tools like parted/libparted;
// here we just update our data structures
void UpdateDiskLayout(DISKENTRY *disk_entry)
{
    if (!disk_entry)
        return;

    // Example logic: mark disk dirty if layout changed
    disk_entry->dirty = true;

    // Normally, you would call libparted or write partition table with ioctl
    // For example, you might use ioctl(fd, BLKPG_ADD_PARTITION, ...) or similar,
    // but it's complex; you usually use parted tools or write MBR/GPT sectors manually.

    // For demonstration, we'll just print the partitions
    PARTENTRY *part = disk_entry->primary_partitions;
    int index = 0;

    printf("Primary partitions for disk %s:\n", disk_entry->device_path);
    while (part) {
        printf(" Partition %d: start %llu sectors, count %llu sectors, type 0x%x\n",
               index + 1, part->start_sector, part->sector_count, part->partition_type);
        part = part->next;
        index++;
    }

    // Same for logical partitions
    part = disk_entry->logical_partitions;
    index = 0;
    printf("Logical partitions for disk %s:\n", disk_entry->device_path);
    while (part) {
        printf(" Logical %d: start %llu sectors, count %llu sectors, type 0x%x\n",
               index + 1, part->start_sector, part->sector_count, part->partition_type);
        part = part->next;
        index++;
    }

    // Reset dirty after update simulation
    disk_entry->dirty = false;
}

// -- Write partitions to disk (simulate with printing)
int WritePartitions(DISKENTRY *disk_entry)
{
    if (!disk_entry)
        return -1;

    if (!disk_entry->dirty) {
        printf("Disk layout not dirty, nothing to write.\n");
        return 0;
    }

    // WARNING: Real writing requires raw disk access and proper partition table building.
    // On Linux, use libparted or manually write MBR/GPT sectors.

    printf("Writing partitions to disk %s (simulated)...\n", disk_entry->device_path);

    PARTENTRY *part = disk_entry->primary_partitions;
    while (part) {
        if (part->is_partitioned) {
            printf(" Writing partition %d at start %llu sectors, size %llu sectors, type 0x%x\n",
                   part->partition_number, part->start_sector, part->sector_count, part->partition_type);
        }
        part = part->next;
    }

    part = disk_entry->logical_partitions;
    while (part) {
        if (part->is_partitioned) {
            printf(" Writing logical partition %d at start %llu sectors, size %llu sectors, type 0x%x\n",
                   part->partition_number, part->start_sector, part->sector_count, part->partition_type);
        }
        part = part->next;
    }

    // Mark disk as clean after "writing"
    disk_entry->dirty = false;

    return 0;
}


// Helper: get previous unpartitioned partition entry in a linked list
PARTENTRY *GetPrevUnpartitionedEntry(PARTENTRY *part_entry)
{
    if (!part_entry || !part_entry->disk_entry)
        return NULL;

    PARTENTRY *current = NULL;
    PARTENTRY *prev = NULL;
    PARTENTRY *head = part_entry->logical_partition ? part_entry->disk_entry->logical_partitions
                                                   : part_entry->disk_entry->primary_partitions;

    current = head;
    while (current && current != part_entry) {
        if (!current->is_partitioned)
            prev = current;
        current = current->next;
    }

    return prev;
}

// Helper: get next unpartitioned partition entry in a linked list
PARTENTRY *GetNextUnpartitionedEntry(PARTENTRY *part_entry)
{
    if (!part_entry)
        return NULL;

    PARTENTRY *current = part_entry->next;

    while (current) {
        if (!current->is_partitioned)
            return current;
        current = current->next;
    }

    return NULL;
}

// Remove volume entry from a linked list and free
typedef struct _VOLENTRY {
    char device_path[256];
    char label[256];
    char fs_type[256];
    unsigned long long size_bytes;
    struct _VOLENTRY *next;
} VOLENTRY;

VOLENTRY *VolumeListHead = NULL;
VOLENTRY *CurrentVolume = NULL;

void RemoveVolume(VOLENTRY *volume)
{
    if (!volume)
        return;

    VOLENTRY **indirect = &VolumeListHead;
    while (*indirect && *indirect != volume) {
        indirect = &(*indirect)->next;
    }

    if (*indirect == volume) {
        *indirect = volume->next;
    }

    if (CurrentVolume == volume) {
        CurrentVolume = NULL;
    }

    free(volume);
}


