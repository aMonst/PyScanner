// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 在此处引用程序需要的其他头文件
#include "xcgui.h"
#pragma comment(lib, "XCGUI.lib")

typedef struct _tag_PYTHON_MODULES_DATA
{
	TCHAR szModuleName[MAX_PATH];
	TCHAR szVulName[MAX_PATH];
	long nVulLevel;
	struct _tag_PYTHON_MODULES_DATA * NextModule;

	_tag_PYTHON_MODULES_DATA()
	{
		ZeroMemory(szModuleName, MAX_PATH * sizeof(TCHAR));
		ZeroMemory(szVulName, MAX_PATH * sizeof(TCHAR));
		nVulLevel = 0;
		NextModule = NULL;
	}
}PYTHON_MODULES_DATA, *LPPYTHON_MODULES_DATA;

typedef struct _tag_POLICY_INFO
{
	TCHAR szVulName[MAX_PATH];
	BOOL bChecked;
	_tag_POLICY_INFO *next;
}POLICY_INFO, *LPPOLICY_INFO;