/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/interpreter.c
 * PURPOSE:         Reads user input and invokes selected commands.
 * PROGRAMMERS:     Lee Schroeder (original), adapted for Linux by E S-O
 */

#include "diskpart.h"
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ARGS_COUNT 32
#define MAX_STRING_SIZE 1024

// Forward declarations
BOOL exit_main(int argc, wchar_t **argv);
BOOL rem_main(int argc, wchar_t **argv);
BOOL HelpCommand(PCOMMAND cmd);
VOID HelpCommandList(void);

// Command table same as original (not repeated here)...

BOOL InterpretCmd(int argc, wchar_t **argv)
{
    PCOMMAND cmdptr;
    PCOMMAND cmdptr1 = NULL;
    PCOMMAND cmdptr2 = NULL;
    PCOMMAND cmdptr3 = NULL;

    if (argc < 1)
        return true;

    // Exit command
    if (wcscasecmp(argv[0], L"exit") == 0)
        return false;

    // Comment command
    if (wcscasecmp(argv[0], L"rem") == 0)
        return true;

    for (cmdptr = cmds; cmdptr->cmd1; cmdptr++)
    {
        if (!cmdptr1 && cmdptr->cmd1 && wcscasecmp(argv[0], cmdptr->cmd1) == 0)
            cmdptr1 = cmdptr;

        if (!cmdptr2 && argc >= 2 && cmdptr->cmd1 && cmdptr->cmd2 &&
            wcscasecmp(argv[0], cmdptr->cmd1) == 0 &&
            wcscasecmp(argv[1], cmdptr->cmd2) == 0)
            cmdptr2 = cmdptr;

        if (!cmdptr3 && argc >= 3 && cmdptr->cmd1 && cmdptr->cmd2 && cmdptr->cmd3 &&
            wcscasecmp(argv[0], cmdptr->cmd1) == 0 &&
            wcscasecmp(argv[1], cmdptr->cmd2) == 0 &&
            wcscasecmp(argv[2], cmdptr->cmd3) == 0)
            cmdptr3 = cmdptr;
    }

    if (cmdptr3)
    {
        if (!cmdptr3->func)
            return HelpCommand(cmdptr3);
        else
            return cmdptr3->func(argc, argv);
    }
    else if (cmdptr2)
    {
        if (!cmdptr2->func)
            return HelpCommand(cmdptr2);
        else
            return cmdptr2->func(argc, argv);
    }
    else if (cmdptr1)
    {
        if (!cmdptr1->func)
            return HelpCommand(cmdptr1);
        else
            return cmdptr1->func(argc, argv);
    }

    HelpCommandList();

    return true;
}

BOOL InterpretScript(wchar_t *input_line)
{
    wchar_t *args_vector[MAX_ARGS_COUNT];
    int args_count = 0;
    bool bWhiteSpace = true;
    bool bQuote = false;
    wchar_t *ptr = input_line;

    memset(args_vector, 0, sizeof(args_vector));

    while (*ptr != 0)
    {
        if (*ptr == L'"')
            bQuote = !bQuote;

        if ((iswspace(*ptr) && !bQuote) || *ptr == L'\n')
        {
            *ptr = 0;
            bWhiteSpace = true;
        }
        else
        {
            if (bWhiteSpace && !bQuote && args_count < MAX_ARGS_COUNT)
            {
                args_vector[args_count] = ptr;
                args_count++;
            }
            bWhiteSpace = false;
        }
        ptr++;
    }

    return InterpretCmd(args_count, args_vector);
}

void InterpretMain(void)
{
    wchar_t input_line[MAX_STRING_SIZE];
    wchar_t *args_vector[MAX_ARGS_COUNT];
    int args_count = 0;
    bool bWhiteSpace = true;
    bool bQuote = false;
    bool bRun = true;
    wchar_t *ptr;

    while (bRun)
    {
        args_count = 0;
        memset(args_vector, 0, sizeof(args_vector));

        // Linux-friendly prompt
        wprintf(L"diskpart> ");
        fflush(stdout);

        if (!fgetws(input_line, MAX_STRING_SIZE, stdin))
        {
            wprintf(L"\n");
            break; // EOF or error
        }

        bQuote = false;
        ptr = input_line;
        while (*ptr != 0)
        {
            if (*ptr == L'"')
                bQuote = !bQuote;

            if ((iswspace(*ptr) && !bQuote) || *ptr == L'\n')
            {
                *ptr = 0;
                bWhiteSpace = true;
            }
            else
            {
                if (bWhiteSpace && !bQuote && args_count < MAX_ARGS_COUNT)
                {
                    args_vector[args_count] = ptr;
                    args_count++;
                }
                bWhiteSpace = false;
            }
            ptr++;
        }

        bRun = InterpretCmd(args_count, args_vector);
    }
}
