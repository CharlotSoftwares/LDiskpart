/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/expand.c
 * PURPOSE:         Manage partition expansion interactively on Linux.
 * PROGRAMMERS:     Lee Schroeder (ported to Linux by Radiump)
 */

#include "diskpart.h"

BOOL expand_main(INT argc, LPWSTR *argv)
{
    // TODO: Implement partition expansion for Linux using ioctl or libparted
    // For now, stub returning success.

    (void)argc; // silence unused parameter warnings
    (void)argv;

    return TRUE;
}
