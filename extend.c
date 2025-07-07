/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/extend.c
 * PURPOSE:         Manage partition extension interactively on Linux.
 * PROGRAMMERS:     Lee Schroeder (ported to Linux by Radiump)
 */

#include "diskpart.h"

BOOL extend_main(INT argc, LPWSTR *argv)
{
    // TODO: Implement partition extension logic on Linux using ioctl or libparted
    (void)argc; // avoid unused param warnings
    (void)argv;

    return TRUE;
}
