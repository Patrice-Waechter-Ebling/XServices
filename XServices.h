#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <sddl.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>
#include <winsvc.h>

#define WIN32_LEAN_AND_MEAN
#define WIN32_WINNT  0x0500
#pragma comment(lib,"Advapi32")

int RemoveService(char* NomDuService);
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize );
