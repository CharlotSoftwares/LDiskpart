/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/offline.c
 * PURPOSE:         Manages all the partitions of the OS in an interactive way.
 * PROGRAMMERS:     Lee Schroeder, adapted for Linux by Radiump, powered by excessive amounts of black tea
 */

#include "diskpart.h"

/* Linux doesn't have LPWSTR, so define if not defined */
#ifndef LPWSTR
typedef wchar_t* LPWSTR;
#endif

#ifndef BOOL
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

BOOL offline_main(int argc, LPWSTR *argv)
{
    (void)argc;
    (void)argv;

    return TRUE;
}
