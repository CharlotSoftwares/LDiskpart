/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/help.c
 * PURPOSE:         Interactive partition management help system
 * PROGRAMMERS:     Lee Schroeder (original)
 *                  Adapted for Linux by CB
 */

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
#include "diskpart.h"

/* Simulated commands[] array must be provided externally */

static void PrintWide(const wchar_t* str)
{
    // Print wide string to stdout using wprintf
    wprintf(L"%ls", str);
}

static void PrintWideLine(const wchar_t* str)
{
    wprintf(L"%ls\n", str);
}

// Simple formatting, replacing %s for command and description only
static void PrintFormatted(const wchar_t* format, const wchar_t* cmd, const wchar_t* desc)
{
    // Example format might be: L"  %-12s  %s\n"
    wprintf(format, cmd, desc);
}

// Helper: Case-insensitive wide string compare (like _wcsicmp)
static int WideStrCaseCmp(const wchar_t* s1, const wchar_t* s2)
{
    while (*s1 && *s2)
    {
        wchar_t c1 = towlower(*s1);
        wchar_t c2 = towlower(*s2);
        if (c1 != c2)
            return c1 - c2;
        s1++;
        s2++;
    }
    return towlower(*s1) - towlower(*s2);
}

/*
 * HelpCommandList():
 * shows all the available commands and basic descriptions for diskpart
 */
void HelpCommandList(void)
{
    PCOMMAND cmdptr;
    const wchar_t* format = L"  %-12ls  %ls\n";

    PrintWideLine(L"\nDiskPart - Available commands:\n");

    for (cmdptr = cmds; cmdptr && cmdptr->cmd1; cmdptr++)
    {
        if (cmdptr->cmd1 != NULL &&
            cmdptr->cmd2 == NULL &&
            cmdptr->cmd3 == NULL &&
            cmdptr->help != IDS_NONE)
        {
            // For Linux port, let's assume cmds[].help points to a wchar_t* description string directly
            // Instead of loading from resource, just print description stored in cmds struct
            PrintFormatted(format, cmdptr->cmd1, (wchar_t*)cmdptr->help);
        }
    }

    PrintWideLine(L"");
}

bool HelpCommand(PCOMMAND pCommand)
{
    PCOMMAND cmdptr;
    bool bSubCommands = false;
    const wchar_t* format = L"  %-12ls  %ls\n";

    PrintWideLine(L"\n");

    for (cmdptr = cmds; cmdptr && cmdptr->cmd1; cmdptr++)
    {
        if (pCommand->cmd1 != NULL && pCommand->cmd2 == NULL && pCommand->cmd3 == NULL)
        {
            if ((cmdptr->cmd1 != NULL && WideStrCaseCmp(pCommand->cmd1, cmdptr->cmd1) == 0) &&
                (cmdptr->cmd2 != NULL) &&
                (cmdptr->cmd3 == NULL) &&
                (cmdptr->help != IDS_NONE))
            {
                PrintFormatted(format, cmdptr->cmd2, (wchar_t*)cmdptr->help);
                bSubCommands = true;
            }
        }
        else if (pCommand->cmd1 != NULL && pCommand->cmd2 != NULL && pCommand->cmd3 == NULL)
        {
            if ((cmdptr->cmd1 != NULL && WideStrCaseCmp(pCommand->cmd1, cmdptr->cmd1) == 0) &&
                (cmdptr->cmd2 != NULL && WideStrCaseCmp(pCommand->cmd2, cmdptr->cmd2) == 0) &&
                (cmdptr->cmd3 != NULL) &&
                (cmdptr->help != IDS_NONE))
            {
                PrintFormatted(format, cmdptr->cmd3, (wchar_t*)cmdptr->help);
                bSubCommands = true;
            }
        }
        else if (pCommand->cmd1 != NULL && pCommand->cmd2 != NULL && pCommand->cmd3 != NULL)
        {
            if ((cmdptr->cmd1 != NULL && WideStrCaseCmp(pCommand->cmd1, cmdptr->cmd1) == 0) &&
                (cmdptr->cmd2 != NULL && WideStrCaseCmp(pCommand->cmd2, cmdptr->cmd2) == 0) &&
                (cmdptr->cmd3 != NULL && WideStrCaseCmp(pCommand->cmd3, cmdptr->cmd3) == 0) &&
                (cmdptr->help_detail != MSG_NONE))
            {
                // Just print help_detail as a wide string pointer
                PrintWide((wchar_t*)cmdptr->help_detail);
                bSubCommands = true;
            }
        }
    }

    if (!bSubCommands && pCommand->help_detail != MSG_NONE)
    {
        PrintWide((wchar_t*)pCommand->help_detail);
    }

    PrintWideLine(L"");

    return true;
}

bool help_main(int argc, wchar_t* argv[])
{
    PCOMMAND cmdptr;
    PCOMMAND cmdptr1 = NULL;
    PCOMMAND cmdptr2 = NULL;
    PCOMMAND cmdptr3 = NULL;

    if (argc == 1)
    {
        HelpCommandList();
        return true;
    }

    for (cmdptr = cmds; cmdptr && cmdptr->cmd1; cmdptr++)
    {
        if (cmdptr1 == NULL &&
            cmdptr->cmd1 != NULL && WideStrCaseCmp(argv[1], cmdptr->cmd1) == 0)
        {
            cmdptr1 = cmdptr;
        }

        if (cmdptr2 == NULL && argc >= 3 &&
            cmdptr->cmd1 != NULL && WideStrCaseCmp(argv[1], cmdptr->cmd1) == 0 &&
            cmdptr->cmd2 != NULL && WideStrCaseCmp(argv[2], cmdptr->cmd2) == 0)
        {
            cmdptr2 = cmdptr;
        }

        if (cmdptr3 == NULL && argc >= 4 &&
            cmdptr->cmd1 != NULL && WideStrCaseCmp(argv[1], cmdptr->cmd1) == 0 &&
            cmdptr->cmd2 != NULL && WideStrCaseCmp(argv[2], cmdptr->cmd2) == 0 &&
            cmdptr->cmd3 != NULL && WideStrCaseCmp(argv[3], cmdptr->cmd3) == 0)
        {
            cmdptr3 = cmdptr;
        }
    }

    if (cmdptr3 != NULL)
        return HelpCommand(cmdptr3);
    else if (cmdptr2 != NULL)
        return HelpCommand(cmdptr2);
    else if (cmdptr1 != NULL)
        return HelpCommand(cmdptr1);

    HelpCommandList();

    return true;
}
