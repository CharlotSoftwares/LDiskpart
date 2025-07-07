/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/list.c
 * PURPOSE:         Manages all the partitions of the OS in an interactive way.
 * PROGRAMMERS:     Lee Schroeder, adapted for Linux by Mathieux Fontaine
 */

#include "diskpart.h"

#include <wchar.h>
#include <stdio.h>

VOID PrintDisk(PDISKENTRY DiskEntry)
{
    unsigned long long DiskSize;
    unsigned long long FreeSize;
    const wchar_t *lpSizeUnit;
    const wchar_t *lpFreeUnit;

    DiskSize = DiskEntry->SectorCount.QuadPart *
               (unsigned long long)DiskEntry->BytesPerSector;

    if (DiskSize >= 10737418240ULL) /* 10 GB */
    {
        DiskSize = RoundingDivide(DiskSize, 1073741824ULL);
        lpSizeUnit = L"GB";
    }
    else
    {
        DiskSize = RoundingDivide(DiskSize, 1048576ULL);
        if (DiskSize == 0)
            DiskSize = 1;
        lpSizeUnit = L"MB";
    }

    /* FIXME */
    FreeSize = 0;
    lpFreeUnit = L"B";

    // Example format string, adjust as needed
    wprintf(L"%c Disk %d Online %llu %s Free %llu %s\n",
            (CurrentDisk == DiskEntry) ? L'*' : L' ',
            DiskEntry->DiskNumber,
            DiskSize,
            lpSizeUnit,
            FreeSize,
            lpFreeUnit);
}

BOOL ListDisk(INT argc, PWSTR *argv)
{
    PLIST_ENTRY Entry;
    PDISKENTRY DiskEntry;

    wprintf(L"\n");
    wprintf(L"Disk ###  Status         Size     Free\n");
    wprintf(L"-------------------------------------\n");

    Entry = DiskListHead.Flink;
    while (Entry != &DiskListHead)
    {
        DiskEntry = CONTAINING_RECORD(Entry, DISKENTRY, ListEntry);
        PrintDisk(DiskEntry);
        Entry = Entry->Flink;
    }

    wprintf(L"\n\n");
    return TRUE;
}

BOOL ListPartition(INT argc, PWSTR *argv)
{
    PLIST_ENTRY Entry;
    PPARTENTRY PartEntry;
    unsigned long long PartSize;
    unsigned long long PartOffset;
    const wchar_t *lpSizeUnit;
    const wchar_t *lpOffsetUnit;
    unsigned long PartNumber = 1;

    if (CurrentDisk == NULL)
    {
        wprintf(L"No disk selected.\n");
        return TRUE;
    }

    wprintf(L"\n");
    wprintf(L"Partition ###  Type      Size     Offset\n");
    wprintf(L"---------------------------------------\n");

    Entry = CurrentDisk->PrimaryPartListHead.Flink;
    while (Entry != &CurrentDisk->PrimaryPartListHead)
    {
        PartEntry = CONTAINING_RECORD(Entry, PARTENTRY, ListEntry);

        if (PartEntry->PartitionType != 0)
        {
            PartSize = PartEntry->SectorCount.QuadPart * CurrentDisk->BytesPerSector;

            if (PartSize >= 10737418240ULL) /* 10 GB */
            {
                PartSize = RoundingDivide(PartSize, 1073741824ULL);
                lpSizeUnit = L"GB";
            }
            else if (PartSize >= 10485760ULL) /* 10 MB */
            {
                PartSize = RoundingDivide(PartSize, 1048576ULL);
                lpSizeUnit = L"MB";
            }
            else
            {
                PartSize = RoundingDivide(PartSize, 1024ULL);
                lpSizeUnit = L"KB";
            }

            PartOffset = PartEntry->StartSector.QuadPart * CurrentDisk->BytesPerSector;

            if (PartOffset >= 10737418240ULL) /* 10 GB */
            {
                PartOffset = RoundingDivide(PartOffset, 1073741824ULL);
                lpOffsetUnit = L"GB";
            }
            else if (PartOffset >= 10485760ULL) /* 10 MB */
            {
                PartOffset = RoundingDivide(PartOffset, 1048576ULL);
                lpOffsetUnit = L"MB";
            }
            else
            {
                PartOffset = RoundingDivide(PartOffset, 1024ULL);
                lpOffsetUnit = L"KB";
            }

            wprintf(L"%c %3lu  %9ls  %6llu %2ls  %6llu %2ls\n",
                    (CurrentPartition == PartEntry) ? L'*' : L' ',
                    PartNumber++,
                    IsContainerPartition(PartEntry->PartitionType) ? L"Extended" : L"Primary",
                    PartSize,
                    lpSizeUnit,
                    PartOffset,
                    lpOffsetUnit);
        }

        Entry = Entry->Flink;
    }

    Entry = CurrentDisk->LogicalPartListHead.Flink;
    while (Entry != &CurrentDisk->LogicalPartListHead)
    {
        PartEntry = CONTAINING_RECORD(Entry, PARTENTRY, ListEntry);

        if (PartEntry->PartitionType != 0)
        {
            PartSize = PartEntry->SectorCount.QuadPart * CurrentDisk->BytesPerSector;

            if (PartSize >= 10737418240ULL) /* 10 GB */
            {
                PartSize = RoundingDivide(PartSize, 1073741824ULL);
                lpSizeUnit = L"GB";
            }
            else if (PartSize >= 10485760ULL) /* 10 MB */
            {
                PartSize = RoundingDivide(PartSize, 1048576ULL);
                lpSizeUnit = L"MB";
            }
            else
            {
                PartSize = RoundingDivide(PartSize, 1024ULL);
                lpSizeUnit = L"KB";
            }

            PartOffset = PartEntry->StartSector.QuadPart * CurrentDisk->BytesPerSector;

            if (PartOffset >= 10737418240ULL) /* 10 GB */
            {
                PartOffset = RoundingDivide(PartOffset, 1073741824ULL);
                lpOffsetUnit = L"GB";
            }
            else if (PartOffset >= 10485760ULL) /* 10 MB */
            {
                PartOffset = RoundingDivide(PartOffset, 1048576ULL);
                lpOffsetUnit = L"MB";
            }
            else
            {
                PartOffset = RoundingDivide(PartOffset, 1024ULL);
                lpOffsetUnit = L"KB";
            }

            wprintf(L"%c %3lu  Logical   %6llu %2ls  %6llu %2ls\n",
                    (CurrentPartition == PartEntry) ? L'*' : L' ',
                    PartNumber++,
                    PartSize,
                    lpSizeUnit,
                    PartOffset,
                    lpOffsetUnit);
        }

        Entry = Entry->Flink;
    }

    wprintf(L"\n");
    return TRUE;
}

VOID PrintVolume(PVOLENTRY VolumeEntry)
{
    unsigned long long VolumeSize;
    const wchar_t *pszSizeUnit;
    const wchar_t *pszVolumeType;

    VolumeSize = VolumeEntry->Size.QuadPart;
    if (VolumeSize >= 10737418240ULL) /* 10 GB */
    {
        VolumeSize = RoundingDivide(VolumeSize, 1073741824ULL);
        pszSizeUnit = L"GB";
    }
    else if (VolumeSize >= 10485760ULL) /* 10 MB */
    {
        VolumeSize = RoundingDivide(VolumeSize, 1048576ULL);
        pszSizeUnit = L"MB";
    }
    else
    {
        VolumeSize = RoundingDivide(VolumeSize, 1024ULL);
        pszSizeUnit = L"KB";
    }

    switch (VolumeEntry->VolumeType)
    {
        case VOLUME_TYPE_CDROM:
            pszVolumeType = L"DVD";
            break;
        case VOLUME_TYPE_PARTITION:
            pszVolumeType = L"Partition";
            break;
        case VOLUME_TYPE_REMOVABLE:
            pszVolumeType = L"Removable";
            break;
        default:
            pszVolumeType = L"Unknown";
            break;
    }

    wprintf(L"%c Volume %d %lc %ls %ls %ls %llu %ls\n",
            (CurrentVolume == VolumeEntry) ? L'*' : L' ',
            VolumeEntry->VolumeNumber,
            VolumeEntry->DriveLetter,
            (VolumeEntry->pszLabel) ? VolumeEntry->pszLabel : L"",
            (VolumeEntry->pszFilesystem) ? VolumeEntry->pszFilesystem : L"",
            pszVolumeType,
            VolumeSize,
            pszSizeUnit);
}

BOOL ListVolume(INT argc, PWSTR *argv)
{
    PLIST_ENTRY Entry;
    PVOLENTRY VolumeEntry;

    wprintf(L"\n");
    wprintf(L"Volume ###  Ltr  Label       Fs     Type       Size\n");
    wprintf(L"-------------------------------------------------\n");

    Entry = VolumeListHead.Flink;
    while (Entry != &VolumeListHead)
    {
        VolumeEntry = CONTAINING_RECORD(Entry, VOLENTRY, ListEntry);

        PrintVolume(VolumeEntry);

        Entry = Entry->Flink;
    }

    wprintf(L"\n");
    return TRUE;
}

BOOL ListVirtualDisk(INT argc, PWSTR *argv)
{
    wprintf(L"ListVirtualDisk()!\n");
    return TRUE;
}

