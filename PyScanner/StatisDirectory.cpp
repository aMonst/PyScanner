#include "StdAfx.h"
#include "StatisDirectory.h"
#include<strsafe.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

CStatisDirectory::CStatisDirectory(void)
{
}

CStatisDirectory::~CStatisDirectory(void)
{
}

BOOL CStatisDirectory::GetAppPath(OUT LPTSTR lpPath, IN size_t sNumberOftchars)
{
	TCHAR szAppDir[MAX_PATH] = _T("");
	GetModuleFileName(GetModuleHandle(NULL), szAppDir, MAX_PATH);
	size_t dwPathLength = 0;

	StringCchLength(szAppDir, MAX_PATH, &dwPathLength);
	for (int i = dwPathLength - 1; i > 0; i--)
	{
		if(szAppDir[i] == _T('\\'))
		{
			szAppDir[i + 1] = _T('\0');
			break;
		}
	}

	HRESULT hRes = StringCchCopy(lpPath, sNumberOftchars, szAppDir);
	return SUCCEEDED(hRes)? TRUE : FALSE;
}

BOOL CStatisDirectory::PathJoin(OUT LPTSTR lpPath, IN size_t sNumberOftchars, IN LPCTSTR lpDesPath, IN LPCTSTR lpSrcPath)
{
	TCHAR szPath[MAX_PATH] = _T("");
	PathCombine(szPath, lpDesPath, lpSrcPath);

	HRESULT hRes = StringCchCopy(lpPath, sNumberOftchars, szPath);
	return SUCCEEDED(hRes)? TRUE : FALSE;
}