/*
 * PROJECT:         ReactOS DiskPart (Linux port)
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/system/diskpart/misc.c
 * PURPOSE:         Manages all the partitions of the OS in an interactive way.
 * PROGRAMMERS:     Eric Kohl, adapted for Linux by Radiump
 */

#include "diskpart.h"

#include <wchar.h>
#include <wctype.h>  // For iswdigit, iswxdigit
#include <stdlib.h>  // For malloc, free
#include <string.h>  // For wcslen, wcsncpy, wcscpy

/* FUNCTIONS ******************************************************************/

BOOL
IsDecString(
    _In_ PWSTR pszDecString)
{
    PWSTR ptr;

    if ((pszDecString == NULL) || (*pszDecString == L'\0'))
        return FALSE;

    ptr = pszDecString;
    while (*ptr != L'\0')
    {
        if (!iswdigit(*ptr))
            return FALSE;

        ptr++;
    }

    return TRUE;
}


BOOL
IsHexString(
    _In_ PWSTR pszHexString)
{
    PWSTR ptr;

    if ((pszHexString == NULL) || (*pszHexString == L'\0'))
        return FALSE;

    ptr = pszHexString;
    while (*ptr != L'\0')
    {
        if (!iswxdigit(*ptr))
            return FALSE;

        ptr++;
    }

    return TRUE;
}


BOOL
HasPrefix(
    _In_ PWSTR pszString,
    _In_ PWSTR pszPrefix,
    _Out_opt_ PWSTR *ppszSuffix)
{
    INT nPrefixLength;
    int ret;

    nPrefixLength = wcslen(pszPrefix);
    ret = wcsncasecmp(pszString, pszPrefix, nPrefixLength); // Linux equivalent of _wcsnicmp
    if ((ret == 0) && (ppszSuffix != NULL))
        *ppszSuffix = &pszString[nPrefixLength];

    return (ret == 0);
}


ULONGLONG
RoundingDivide(
    _In_ ULONGLONG Dividend,
    _In_ ULONGLONG Divisor)
{
    return (Dividend + Divisor / 2) / Divisor;
}


PWSTR
DuplicateQuotedString(
    _In_ PWSTR pszInString)
{
    PWSTR pszOutString = NULL;
    PWSTR pStart, pEnd;
    INT nLength;

    if ((pszInString == NULL) || (pszInString[0] == L'\0'))
        return NULL;

    if (pszInString[0] == L'"')
    {
        if (pszInString[1] == L'\0')
            return NULL;

        pStart = &pszInString[1];
        pEnd = wcschr(pStart, L'"');
        if (pEnd == NULL)
        {
            nLength = wcslen(pStart);
        }
        else
        {
            nLength = (int)(pEnd - pStart);
        }
    }
    else
    {
        pStart = pszInString;
        nLength = (int)wcslen(pStart);
    }

    pszOutString = (PWSTR)malloc((nLength + 1) * sizeof(wchar_t));
    if (pszOutString == NULL)
        return NULL;

    wcsncpy(pszOutString, pStart, nLength);
    pszOutString[nLength] = L'\0';

    return pszOutString;
}


PWSTR
DuplicateString(
    _In_ PWSTR pszInString)
{
    PWSTR pszOutString = NULL;
    INT nLength;

    if ((pszInString == NULL) || (pszInString[0] == L'\0'))
        return NULL;

    nLength = (INT)wcslen(pszInString);
    pszOutString = (PWSTR)malloc((nLength + 1) * sizeof(wchar_t));
    if (pszOutString == NULL)
        return NULL;

    wcscpy(pszOutString, pszInString);

    return pszOutString;
}

