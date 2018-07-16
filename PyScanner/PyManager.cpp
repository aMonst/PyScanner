#include "StdAfx.h"
#include "PyManager.h"
#include <strsafe.h>
#include <AtlConv.h>
#include "StatisDirectory.h"

CPyManager::CPyManager(void)
{
	m_pModuleHead = NULL;
}

CPyManager::~CPyManager(void)
{
	LPPYTHON_MODULES_DATA pTemp = m_pModuleHead;
	while (NULL != pTemp)
	{
		m_pModuleHead = pTemp->NextModule;
		delete pTemp;
		pTemp = m_pModuleHead;
	}
}

BOOL CPyManager::Init()
{
	Py_Initialize();
	
	return Py_IsInitialized();
}

void CPyManager::UnInit()
{
	Py_Finalize();
}

void CPyManager::LoadPlugins(LPCTSTR lpPath)
{
	TCHAR szAppDir[MAX_PATH] = _T("");
	TCHAR szFindFilter[MAX_PATH] = _T("");

	CStatisDirectory::GetAppPath(szAppDir, MAX_PATH);
	CStatisDirectory::PathJoin(m_szPluginsPath, MAX_PATH, szAppDir, lpPath);
	CStatisDirectory::PathJoin(szFindFilter, MAX_PATH, m_szPluginsPath, _T("*.py"));
	
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = FindFirstFile(szFindFilter, &wfd);
	while (TRUE)
	{
		LPPYTHON_MODULES_DATA pModule = new PYTHON_MODULES_DATA;
		StringCchCopy(pModule->szModuleName, MAX_PATH, wfd.cFileName);
		//末尾都是.py 结尾，所以这里默认去掉末尾的3个字符
		size_t dwPathLength = 0;
		StringCchLength(pModule->szModuleName, MAX_PATH, &dwPathLength);
		(pModule->szModuleName)[dwPathLength - 3] = _T('\0');
		ChangePyWorkPath(m_szPluginsPath);
		GetModuleInformation(pModule->szModuleName, pModule->szVulName, pModule->nVulLevel);
		if (NULL != m_pModuleHead)
		{
			pModule->NextModule = m_pModuleHead;
		}

		m_pModuleHead = pModule;

		if(!FindNextFile(hFind, &wfd))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				break;
			}
		}
	}
}

void CPyManager::GetModuleInformation(IN LPCTSTR lpPyFileName, OUT LPTSTR lpVulName, OUT long& level)
{
	USES_CONVERSION;

	PyObject *pModule = PyImport_ImportModule(T2A(lpPyFileName)); //加载模块
	if (NULL == pModule)
	{
		g_OutputString(_T("加载模块[%s]失败"), lpPyFileName);
		goto __CLEAN_UP;
	}

	PyObject *pGetInformationFunc = PyObject_GetAttrString(pModule, "getInformation"); // 加载模块中的函数
	if (NULL == pGetInformationFunc || !PyCallable_Check(pGetInformationFunc))
	{
		g_OutputString(_T("加载函数[%s]失败"), _T("getInformation"));
		goto __CLEAN_UP;
	}

	PyObject *PyResult = PyObject_CallObject(pGetInformationFunc, NULL);
	if (NULL != PyResult)
	{
		PyObject *pVulNameObj = PyTuple_GetItem(PyResult, 0);
		PyObject *pVulLevelObj = PyTuple_GetItem(PyResult, 1);
		
		//获取漏洞的名称信息
		int nStrSize = 0;
		LPTSTR pVulName = PyUnicode_AsWideCharString(pVulNameObj, &nStrSize);
		StringCchCopy(lpVulName, MAX_PATH, pVulName);
		PyMem_Free(pVulName);

		//获取漏洞的危险等级
		level = PyLong_AsLong(pVulLevelObj);

		Py_DECREF(pVulNameObj);
		Py_DECREF(pVulLevelObj);
	}

	//解除Python对象的引用, 以便Python进行垃圾回收
__CLEAN_UP:
	Py_DECREF(pModule);
	Py_DECREF(pGetInformationFunc);
	Py_DECREF(PyResult);

}

void CPyManager::ChangePyWorkPath(LPCTSTR lpWorkPath)
{
	TCHAR szWorkPath[MAX_PATH + 64] = _T("");
	StringCchCopy(szWorkPath, MAX_PATH + 64, _T("sys.path.append(\""));
	StringCchCat(szWorkPath, MAX_PATH + 64, lpWorkPath);
	StringCchCat(szWorkPath, MAX_PATH + 64, _T("\")"));

	PyRun_SimpleString("import sys");
	USES_CONVERSION;

	int nRet = PyRun_SimpleString(T2A(szWorkPath));
	if (nRet != 0)
	{
		return;
	}
}

BOOL CPyManager::GetPythonVersion(LPTSTR lpVersion, size_t sBuffSize)
{
	PyObject *pModule = PyImport_ImportModule("platform");
	if (NULL == pModule)
	{
		return FALSE;
	}

	PyObject *pPythonVerionFunc = PyObject_GetAttrString(pModule, "python_version");
	if (NULL == pPythonVerionFunc || ! PyCallable_Check(pPythonVerionFunc))
	{
		Py_DECREF(pModule);
		if (NULL != pPythonVerionFunc)
		{
			Py_DECREF(pPythonVerionFunc);
		}
		return FALSE;
	}

	PyObject *pResult = PyObject_CallObject(pPythonVerionFunc, NULL);
	if (NULL == pResult)
	{
		Py_DECREF(pModule);
		Py_DECREF(pPythonVerionFunc);

		return FALSE;
	}

	int nVersionSize = 0;
	LPTSTR pVersionInfo = PyUnicode_AsWideCharString(pResult, &nVersionSize); //将python字符串转化为C++字符串，最后使用PyMem_Free 释放内存

	HRESULT hRes = StringCchCopy(lpVersion, sBuffSize, pVersionInfo);
	PyMem_Free(pVersionInfo);

	return SUCCEEDED(hRes)? TRUE : FALSE;
}

LPPYTHON_MODULES_DATA CPyManager::GetFirstScript()
{
	return m_pModuleHead;
}

BOOL CPyManager::CallScanMethod(IN LPPYTHON_MODULES_DATA pPyModule, IN LPCTSTR lpUrl, IN LPCTSTR lpRequestMethod, OUT LPTSTR lpHasVulUrl, int BuffSize)
{
	USES_CONVERSION;
	//加载模块
	PyObject* pModule = PyImport_ImportModule(T2A(pPyModule->szModuleName));
	if (NULL == pModule)
	{
		g_OutputString(_T("加载模块[%s]失败!!!"), pPyModule->szModuleName);
		return FALSE;
	}
	
	//加载模块
	PyObject *pyScanMethod = PyObject_GetAttrString(pModule, "startScan");
	if (NULL == pyScanMethod || !PyCallable_Check(pyScanMethod))
	{
		Py_DECREF(pModule);
		g_OutputString(_T("加载函数[%s]失败!!!"), _T("startScan"));
		return FALSE;
	}

	//加载参数
	PyObject* pArgs = Py_BuildValue("ss", T2A(lpUrl), T2A(lpRequestMethod));

	PyObject *pRes = PyObject_CallObject(pyScanMethod, pArgs);
	Py_DECREF(pArgs);

	if (NULL == pRes)
	{
		g_OutputString(_T("调用函数[%s]失败!!!!"), _T("startScan"));
		return FALSE;
	}

	//如果是元组，那么Python脚本返回的是两个参数，证明发现漏洞
	if (PyTuple_Check(pRes))
	{
		PyObject* pHasVul = PyTuple_GetItem(pRes, 0);
		long bHasVul = PyLong_AsLong(pHasVul);
		Py_DECREF(pHasVul);

		if (bHasVul != 0)
		{
			PyObject* pyUrl = PyTuple_GetItem(pRes, 1);
			int nSize = 0;
			LPWSTR pszUrl = PyUnicode_AsWideCharString(pyUrl, &nSize);
			Py_DECREF(pyUrl);

			StringCchCopy(lpHasVulUrl, BuffSize, pszUrl);
			PyMem_Free(pszUrl);

			return TRUE;
		}
	}
	
	Py_DECREF(pRes);
	return FALSE;
}