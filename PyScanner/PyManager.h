#pragma once
#include <Python.h>
#include "stdafx.h"

void g_OutputString(LPCTSTR lpFormat, ...);
#define METHOD_LENGTH 10
class CPyManager
{
public:
	CPyManager(void);
	~CPyManager(void);
	BOOL Init();
	void UnInit();
	void LoadPlugins(IN LPCTSTR lpPath);
	void GetModuleInformation(IN LPCTSTR lpPyFileName, OUT LPTSTR lpModuleName, OUT long& level);
	void ChangePyWorkPath(IN LPCTSTR lpWorkPath);
	BOOL GetPythonVersion(OUT LPTSTR lpVersion, IN size_t sBuffSize);
	LPPYTHON_MODULES_DATA GetFirstScript();
	BOOL CallScanMethod(IN LPPYTHON_MODULES_DATA pPyModule, IN LPCTSTR lpUrl, IN LPCTSTR lpRequestMethod, OUT LPTSTR lpHasVulUrl, int BuffSize);
protected:
	LPPYTHON_MODULES_DATA m_pModuleHead;
	TCHAR m_szPluginsPath[MAX_PATH];
};
