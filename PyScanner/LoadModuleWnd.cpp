#include "StdAfx.h"
#include "LoadModuleWnd.h"
#include "StatisDirectory.h"
#include <strsafe.h>
#include <stdio.h>

CLoadModuleWnd::CLoadModuleWnd(void)
{
	m_hGroupTemp = NULL;
}

CLoadModuleWnd::~CLoadModuleWnd(void)
{
	if (NULL != m_hGroupTemp)
	{
		XTemp_Destroy(m_hGroupTemp);
	}
}

void CLoadModuleWnd::Init()
{
	HXCGUI hWnd = XC_LoadLayout(L"..\\skin\\loadModule.xml");
	if (!XC_IsHWINDOW(hWnd))
	{
		return ;
	}

	m_hWnd = (HWINDOW)hWnd;
	HFONTX hFont = XFont_Create(12);
	HXCGUI hTitleText = XC_GetObjectByID(m_hWnd, IDR_TITLE);
	if (XC_IsShape(hTitleText))
	{
		XShapeText_SetFont(hTitleText, hFont);
	}

	//系统按钮
	HXCGUI hMinBtn = XC_GetObjectByID(m_hWnd, IDC_MIN_BTN);
	HXCGUI hCloseBtn = XC_GetObjectByID(m_hWnd, IDC_CLOSE_BTN);

	if (XC_IsHELE(hMinBtn) &&
		XC_IsHELE(hCloseBtn))
	{
		XBtn_AddBkImage((HELE)hMinBtn, button_state_leave, XImage_LoadFile(L"../SysBtn/MinNormal.bmp"));
		XBtn_AddBkImage((HELE)hMinBtn, button_state_stay, XImage_LoadFile(L"../SysBtn/MinNormal.bmp"));
		XBtn_AddBkImage((HELE)hMinBtn, button_state_down, XImage_LoadFile(L"../SysBtn/MinFocus.bmp"));

		XBtn_AddBkImage((HELE)hCloseBtn, button_state_leave, XImage_LoadFile(L"../SysBtn/CloseNormal.bmp"));
		XBtn_AddBkImage((HELE)hCloseBtn, button_state_stay, XImage_LoadFile(L"../SysBtn/CloseNormal.bmp"));
		XBtn_AddBkImage((HELE)hCloseBtn, button_state_down, XImage_LoadFile(L"../SysBtn/CloseFocus.bmp"));
	}

	LoadModuleInfo();

	XWnd_AdjustLayout(m_hWnd);
	XModalWnd_DoModal(m_hWnd);
}

void CLoadModuleWnd::LoadModuleInfo()
{
	HELE hTree = (HELE)XC_GetObjectByID(m_hWnd, IDC_TREE);
	if (!XC_IsHELE(hTree))
	{
		return;
	}

	XTree_SetItemTemplateXML(hTree,L"../skin/ModuleTreeItem.xml");
	m_hGroupTemp = XTemp_Load(XC_TREE, L"../skin/VulInfoTemp.xml");
	XTree_EnableExpand(hTree, FALSE);

	HXCGUI hTreeAdapter = XAdTree_Create();
	XTree_BindAdapter(hTree, hTreeAdapter);
	XAdTree_AddColumn(hTreeAdapter, L"name");

	TCHAR szAppPath[MAX_PATH] = _T("");
	CStatisDirectory::GetAppPath(szAppPath, MAX_PATH);
	TCHAR szPolicyPath[MAX_PATH] = _T("");
	CStatisDirectory::PathJoin(szPolicyPath, MAX_PATH, szAppPath, _T("..\\policy\\*.config"));

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFind = FindFirstFile(szPolicyPath, &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		return;
	}
	
	int nRoot = XAdTree_InsertItemText(hTreeAdapter, L"策略");
	XEle_RegEventCPP(hTree, XE_TREE_TEMP_CREATE_END, &CLoadModuleWnd::OnTreeTemplateCreateEnd);

	while (TRUE)
	{
		TCHAR szPolicyName[MAX_PATH] = _T("");
		StringCchCopy(szPolicyName, MAX_PATH, wfd.cFileName);
		size_t sNameLength = 0;
		StringCchLength(szPolicyName, MAX_PATH, &sNameLength);
		szPolicyName[sNameLength - 7] = _T('\0');

		int nNote = XAdTree_InsertItemText(hTreeAdapter, szPolicyName, nRoot);
		TCHAR szPolicyFile[MAX_PATH] = _T("");
		CStatisDirectory::PathJoin(szPolicyFile, MAX_PATH, szAppPath, _T("..\\policy"));
		CStatisDirectory::PathJoin(szPolicyFile, MAX_PATH, szPolicyFile, wfd.cFileName);

		LoadVuls(hTreeAdapter, nNote, szPolicyFile);

		if(!FindNextFile(hFind, &wfd))
		{
			break;
		}
	}
}

void CLoadModuleWnd::LoadVuls(HXCGUI hAdapter, int nNote, LPTSTR lpszFile)
{
	FILE *f = _tfopen(lpszFile, _T("r"));
	char szBuf[MAX_PATH] = "";
	

	while (!feof(f))
	{
		fgets(szBuf, MAX_PATH, f);
		TCHAR szVulName[MAX_PATH] = _T("");
		if(UTF8_2_UNICODE(szVulName, MAX_PATH, szBuf))
		{
			size_t sLength = 0;
			StringCchLength(szVulName, MAX_PATH, &sLength);
			BOOL bChecked = szVulName[sLength - 2] == _T('1');
			szVulName[sLength - 2] = _T('\0');

			XAdTree_InsertItemText(hAdapter, szVulName, nNote);
		}
	}
}

BOOL UTF8_2_UNICODE(LPTSTR lpBuff, DWORD dwBuffSize, LPCSTR lpUTF8Str)
{
	LPTSTR lpDestBuf = NULL;
	int nNeedBuffSize = MultiByteToWideChar(CP_UTF8, 0, lpUTF8Str, -1, lpDestBuf, 0);
	lpDestBuf = new TCHAR[nNeedBuffSize];
	
	nNeedBuffSize = MultiByteToWideChar(CP_UTF8, 0, lpUTF8Str, -1, lpDestBuf, nNeedBuffSize);
	if (nNeedBuffSize == 0)
	{
		delete[] lpDestBuf;
		return FALSE;
	}

	HRESULT hRes = StringCchCopy(lpBuff, dwBuffSize, lpDestBuf);
	delete[] lpDestBuf;

	return SUCCEEDED(hRes);
}

int CLoadModuleWnd::OnTreeTemplateCreateEnd(tree_item_i* pItem,BOOL *pbHandled)
{
	HELE hTree = (HELE)XC_GetObjectByID(m_hWnd, IDC_TREE);
	if (!XC_IsHELE(hTree))
	{
		*pbHandled = TRUE;
		return 0 ;
	}

	int nChildID = XTree_GetFirstChildItem(hTree, pItem->nID);
	if (nChildID == XC_ID_ERROR)
	{
	}

	*pbHandled = TRUE;
	return 0;
}