#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <parted/parted.h>

// Parse size string like "size=100" (MB)
static unsigned long long parse_size_mb(const char *arg) {
    if (strncmp(arg, "size=", 5) == 0) {
        return strtoull(arg + 5, NULL, 10);
    }
    return 0;
}

// Parse id= hex partition type code (for MBR)
static unsigned int parse_id(const char *arg) {
    if (strncmp(arg, "id=", 3) == 0) {
        return (unsigned int)strtoul(arg + 3, NULL, 16);
    }
    return 0x0;
}

static void usage(const char *progname) {
    printf("Usage: %s <device> <type> [options]\n", progname);
    printf("  <device> : e.g. /dev/sda\n");
    printf("  <type>   : primary | extended | logical\n");
    printf("  options  : size=<MB> id=<hex partition id> (id only for MBR)\n");
}

// Convert MB to sectors (assume 512 bytes per sector)
static PedSector size_mb_to_sectors(unsigned long long mb) {
    return (PedSector)(mb * 1024 * 1024 / 512);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argv[0]);
        return 1;
    }

    const char *device_path = argv[1];
    const char *part_type_str = argv[2];
    unsigned long long size_mb = 0;
    unsigned int part_id = 0x0;  // default for MBR

    // Parse options
    for (int i = 3; i < argc; i++) {
        if (strncmp(argv[i], "size=", 5) == 0) {
            size_mb = parse_size_mb(argv[i]);
        } else if (strncmp(argv[i], "id=", 3) == 0) {
            part_id = parse_id(argv[i]);
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    PedDevice *dev = ped_device_get(device_path);
    if (!dev) {
        fprintf(stderr, "Failed to open device %s\n", device_path);
        return 1;
    }

    if (!ped_device_open(dev)) {
        fprintf(stderr, "Failed to open device %s\n", device_path);
        ped_device_destroy(dev);
        return 1;
    }

    PedDisk *disk = ped_disk_new(dev);
    if (!disk) {
        fprintf(stderr, "Failed to read disk label on %s\n", device_path);
        ped_device_close(dev);
        ped_device_destroy(dev);
        return 1;
    }

    // Determine partition type enum
    PedPartitionType part_type = NULL;
    if (strcmp(part_type_str, "primary") == 0) {
        part_type = ped_partition_type_get(disk, PED_PARTITION_PRIMARY);
    } else if (strcmp(part_type_str, "extended") == 0) {
        part_type = ped_partition_type_get(disk, PED_PARTITION_EXTENDED);
    } else if (strcmp(part_type_str, "logical") == 0) {
        part_type = ped_partition_type_get(disk, PED_PARTITION_LOGICAL);
    } else {
        fprintf(stderr, "Unknown partition type: %s\n", part_type_str);
        ped_disk_destroy(disk);
        ped_device_close(dev);
        ped_device_destroy(dev);
        return 1;
    }

    // Find free space on disk
    PedGeometry *geom = ped_disk_get_free_space(disk);
    if (!geom) {
        fprintf(stderr, "No free space available on %s\n", device_path);
        ped_disk_destroy(disk);
        ped_device_close(dev);
        ped_device_destroy(dev);
        return 1;
    }

    PedSector start_sector = geom->start;
    PedSector end_sector = geom->end;

    // If size specified, adjust end sector accordingly
    if (size_mb > 0) {
        PedSector size_sectors = size_mb_to_sectors(size_mb);
        if (start_sector + size_sectors - 1 > end_sector) {
            fprintf(stderr, "Not enough free space for requested size %llu MB\n", size_mb);
            ped_disk_destroy(disk);
            ped_device_close(dev);
            ped_device_destroy(dev);
            return 1;
        }
        end_sector = start_sector + size_sectors - 1;
    }

    PedPartition *new_part = ped_partition_new(disk, part_type, part_id, start_sector, end_sector);
    if (!new_part) {
        fprintf(stderr, "Failed to create new partition\n");
        ped_disk_destroy(disk);
        ped_device_close(dev);
        ped_device_destroy(dev);
        return 1;
    }

    if (!ped_disk_add_partition(disk, new_part, ped_constraint_any(dev))) {
        fprintf(stderr, "Failed to add partition to disk\n");
        ped_partition_destroy(new_part);
        ped_disk_destroy(disk);
        ped_device_close(dev);
        ped_device_destroy(dev);
        return 1;
    }

    if (!ped_disk_commit_to_dev(disk)) {
        fprintf(stderr, "Failed to write partition table to disk\n");
        ped_partition_destroy(new_part);
        ped_disk_destroy(disk);
        ped_device_close(dev);
        ped_device_destroy(dev);
        return 1;
    }

    printf("Partition created successfully: %s %s size %llu MB\n", device_path, part_type_str, size_mb);

    ped_disk_destroy(disk);
    ped_device_close(dev);
    ped_device_destroy(dev);

    return 0;
}

