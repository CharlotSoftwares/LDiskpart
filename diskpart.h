/*
 * PROJECT:         ReactOS DiskPart (Linux Port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            diskpart.h
 * PURPOSE:         Common definitions and declarations for the DiskPart Linux port.
 * PROGRAMMERS:     Ported by Radiump to Linux
 */

#ifndef DISKPART_H
#define DISKPART_H

/* INCLUDES ******************************************************************/

#include <parted/parted.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/* DEBUG STUB ****************************************************************/
#define DPRINT(...) do {} while (0) // Replace with printf if needed

/* TYPES *********************************************************************/

typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef uint64_t ULONGLONG;
typedef uint64_t ULONG64;
typedef uint32_t ULONG;
typedef uint16_t USHORT;
typedef uint8_t  UCHAR;
typedef int32_t  NTSTATUS;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

#define MAX_STRING_SIZE 1024
#define MAX_ARGS_COUNT  256
#define MAX_PATH        260

/* Doubly Linked List ********************************************************/

typedef struct ListEntry {
    struct ListEntry *Flink;
    struct ListEntry *Blink;
} ListEntry;

static inline void InitializeListHead(ListEntry *list) {
    list->Flink = list;
    list->Blink = list;
}

static inline bool IsListEmpty(const ListEntry *list) {
    return list->Flink == list;
}

static inline void InsertTailList(ListEntry *list, ListEntry *entry) {
    entry->Blink = list->Blink;
    entry->Flink = list;
    list->Blink->Flink = entry;
    list->Blink = entry;
}

static inline void RemoveEntryList(ListEntry *entry) {
    entry->Blink->Flink = entry->Flink;
    entry->Flink->Blink = entry->Blink;
}

/* Simplified UNICODE_STRING *************************************************/

typedef struct _UNICODE_STRING {
    uint16_t Length;
    uint16_t MaximumLength;
    char *Buffer;
} UNICODE_STRING;

/* FORMAT STATE & VOLUME TYPES ***********************************************/

typedef enum _FORMATSTATE {
    Unformatted,
    UnformattedOrDamaged,
    UnknownFormat,
    Preformatted,
    Formatted
} FORMATSTATE;

typedef enum _VOLUME_TYPE {
    VOLUME_TYPE_CDROM,
    VOLUME_TYPE_PARTITION,
    VOLUME_TYPE_REMOVABLE,
    VOLUME_TYPE_UNKNOWN
} VOLUME_TYPE;

/* STRUCT DEFINITIONS ********************************************************/

struct DiskEntry;
struct VolumeEntry;

typedef struct _PARTENTRY {
    ListEntry ListEntry;
    struct DiskEntry *DiskEntry;

    ULONGLONG StartSector;
    ULONGLONG SectorCount;

    BOOL BootIndicator;
    UCHAR PartitionType;
    ULONG OnDiskPartitionNumber;
    ULONG PartitionNumber;
    ULONG PartitionIndex;

    char DriveLetter;
    char VolumeLabel[17];
    char FileSystemName[9];
    FORMATSTATE FormatState;

    BOOL LogicalPartition;
    BOOL IsPartitioned;
    BOOL New;
    BOOL AutoCreate;
    BOOL NeedsCheck;

    void *FileSystem;
} PARTENTRY, *PPARTENTRY;

typedef struct _DISK_LAYOUT_LINUX {
    ULONG Signature;
} DISK_LAYOUT_LINUX, *PDISK_LAYOUT_LINUX;

typedef struct _DISKENTRY {
    ListEntry ListEntry;

    ULONGLONG Cylinders;
    ULONG TracksPerCylinder;
    ULONG SectorsPerTrack;
    ULONG BytesPerSector;

    ULONGLONG SectorCount;
    ULONG SectorAlignment;
    ULONG CylinderAlignment;

    BOOL BiosFound;
    ULONG BiosDiskNumber;

    ULONG DiskNumber;
    USHORT Port;
    USHORT PathId;
    USHORT TargetId;
    USHORT Lun;

    BOOL Dirty;
    BOOL NewDisk;
    BOOL NoMbr;

    UNICODE_STRING DriverName;

    void *LayoutBuffer;

    PPARTENTRY ExtendedPartition;

    ListEntry PrimaryPartListHead;
    ListEntry LogicalPartListHead;

} DISKENTRY, *PDISKENTRY;

typedef struct _VOLENTRY {
    ListEntry ListEntry;

    ULONG VolumeNumber;
    char VolumeName[MAX_PATH];
    char DeviceName[MAX_PATH];

    char DriveLetter;
    char *pszLabel;
    char *pszFilesystem;

    VOLUME_TYPE VolumeType;
    ULONGLONG Size;

    void *pExtents;
} VOLENTRY, *PVOLENTRY;

/* GLOBALS *******************************************************************/

extern ListEntry DiskListHead;
extern ListEntry BiosDiskListHead;
extern ListEntry VolumeListHead;

extern PDISKENTRY CurrentDisk;
extern PPARTENTRY CurrentPartition;
extern PVOLENTRY CurrentVolume;

/* COMMAND DISPATCH **********************************************************/

typedef struct _COMMAND {
    char *cmd1;
    char *cmd2;
    char *cmd3;
    BOOL (*func)(int argc, char **argv);
    int help;
    unsigned int help_detail;
} COMMAND, *PCOMMAND;

extern COMMAND cmds[];

/* FUNCTION PROTOTYPES *******************************************************/

/* Each of these is one command source file */
BOOL active_main(int argc, char **argv);
BOOL add_main(int argc, char **argv);
BOOL assign_main(int argc, char **argv);
BOOL attach_main(int argc, char **argv);
BOOL attributes_main(int argc, char **argv);
BOOL automount_main(int argc, char **argv);
BOOL break_main(int argc, char **argv);
BOOL clean_main(int argc, char **argv);
BOOL compact_main(int argc, char **argv);
BOOL convert_main(int argc, char **argv);

BOOL CreateExtendedPartition(int argc, char **argv);
BOOL CreateLogicalPartition(int argc, char **argv);
BOOL CreatePrimaryPartition(int argc, char **argv);

BOOL DeleteDisk(int argc, char **argv);
BOOL DeletePartition(int argc, char **argv);
BOOL DeleteVolume(int argc, char **argv);

BOOL detach_main(int argc, char **argv);
BOOL DetailDisk(int argc, char **argv);
BOOL DetailPartition(int argc, char **argv);
BOOL DetailVolume(int argc, char **argv);

BOOL DumpDisk(int argc, char **argv);
BOOL DumpPartition(int argc, char **argv);

BOOL expand_main(int argc, char **argv);
BOOL extend_main(int argc, char **argv);
BOOL filesystems_main(int argc, char **argv);
BOOL format_main(int argc, char **argv);
BOOL gpt_main(int argc, char **argv);
BOOL help_main(int argc, char **argv);
void HelpCommandList(void);
BOOL HelpCommand(PCOMMAND pCommand);
BOOL import_main(int argc, char **argv);
BOOL inactive_main(int argc, char **argv);
BOOL InterpretScript(char *line);
BOOL InterpretCmd(int argc, char **argv);
void InterpretMain(void);

BOOL ListDisk(int argc, char **argv);
BOOL ListPartition(int argc, char **argv);
BOOL ListVolume(int argc, char **argv);
BOOL ListVirtualDisk(int argc, char **argv);
void PrintDisk(PDISKENTRY DiskEntry);
void PrintVolume(PVOLENTRY VolumeEntry);

BOOL merge_main(int argc, char **argv);
BOOL IsDecString(char *pszDecString);
BOOL IsHexString(char *pszHexString);
BOOL HasPrefix(char *pszString, char *pszPrefix, char **pszSuffix);
ULONGLONG RoundingDivide(ULONGLONG Dividend, ULONGLONG Divisor);
char *DuplicateQuotedString(char *pszInString);
char *DuplicateString(char *pszInString);

BOOL offline_main(int argc, char **argv);
BOOL online_main(int argc, char **argv);

ULONGLONG AlignDown(ULONGLONG Value, ULONG Alignment);
NTSTATUS CreatePartitionList(void);
void DestroyPartitionList(void);
NTSTATUS CreateVolumeList(void);
void DestroyVolumeList(void);
NTSTATUS WritePartitions(PDISKENTRY DiskEntry);
void UpdateDiskLayout(PDISKENTRY DiskEntry);
PPARTENTRY GetPrevUnpartitionedEntry(PPARTENTRY PartEntry);
PPARTENTRY GetNextUnpartitionedEntry(PPARTENTRY PartEntry);
ULONG GetPrimaryPartitionCount(PDISKENTRY DiskEntry);
NTSTATUS DismountVolume(PPARTENTRY PartEntry);
PVOLENTRY GetVolumeFromPartition(PPARTENTRY PartEntry);
void RemoveVolume(PVOLENTRY VolumeEntry);

BOOL recover_main(int argc, char **argv);
BOOL remove_main(int argc, char **argv);
BOOL repair_main(int argc, char **argv);
BOOL rescan_main(int argc, char **argv);
BOOL retain_main(int argc, char **argv);
BOOL san_main(int argc, char **argv);

BOOL SelectDisk(int argc, char **argv);
BOOL SelectPartition(int argc, char **argv);
BOOL SelectVolume(int argc, char **argv);

BOOL setid_main(int argc, char **argv);
BOOL shrink_main(int argc, char **argv);
BOOL UniqueIdDisk(int argc, char **argv);

#endif /* DISKPART_H */
