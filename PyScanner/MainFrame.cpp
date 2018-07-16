#include "StdAfx.h"
#include "MainFrame.h"
#include <stdio.h>
#include "StatisDirectory.h"
#include <strsafe.h>
#include "Resource.h"
#include "LoadModuleWnd.h"

void g_OutputString(LPCTSTR lpFormat, ...)
{
	va_list args;
	va_start(args, lpFormat);
	TCHAR szBuf[1024] = _T("");
	_vsnwprintf_s(szBuf, 1024, 1024, lpFormat, args);
	va_end(args);

	HXCGUI hMainFrame = XC_GetObjectByID(NULL, IDC_MAIN_FRAME);
	if (XC_IsHWINDOW(hMainFrame))
	{
		HELE hOutputView = (HELE)XC_GetObjectByID((HWINDOW)hMainFrame, IDC_OUTPUT_EDIT);
		if (XC_IsHELE(hOutputView))
		{
			XRichEdit_InsertString(hOutputView,szBuf);
			XRichEdit_InsertString(hOutputView, _T("\r\n"));
		}
	}
}

CMainFrame::CMainFrame(void)
{
	m_hMainWnd = NULL;
	if(!m_PyManager.Init())
	{
		g_OutputString(_T("初始化Python环境失败!!!!!!!!!!"));
	}
}

CMainFrame::~CMainFrame(void)
{
	m_PyManager.UnInit();
}

void CMainFrame::Init()
{

	TCHAR szAppDir[MAX_PATH] = _T("");
	CStatisDirectory::GetAppPath(szAppDir, MAX_PATH);
	CStatisDirectory::PathJoin(szAppDir, MAX_PATH, szAppDir, _T("..\\skin\\layout.xml"));
	HXCGUI hLayout = XC_LoadLayout(szAppDir);
	if (XC_IsHWINDOW(hLayout))
	{
		m_hMainWnd = (HWINDOW)hLayout;
		CreateTitle();
		CreateSysButton();
		CreateMenu();
		CreateOutputView();
		
		TCHAR szPythonVersion[10] = _T("");

		g_OutputString(_T("初始化Python 环境成功!!!!!!!!!!!!!!!!!!"));
		m_PyManager.GetPythonVersion(szPythonVersion, 10);
		g_OutputString(_T("当前加载Python版本为:%s"), szPythonVersion);
		
		m_PyManager.LoadPlugins(_T("..\\plugins"));
		LoadPolicy();
		XWnd_AdjustLayout(m_hMainWnd);
		XWnd_ShowWindow(m_hMainWnd, SW_SHOW);

		//设置开始扫描按钮的响应事件
		HELE hStartBtn = (HELE)XC_GetObjectByID(m_hMainWnd, IDC_START_BTN);
		if (XC_IsHELE(hStartBtn))
		{
			XEle_RegEventCPP(hStartBtn, XE_BNCLICK, &CMainFrame::OnBtnClick);
		}
	}
}

void CMainFrame::CreateTitle()
{
	//设置文本
	HFONTX hFont = XFont_Create(12);
	HXCGUI hTitleText = XC_GetObjectByID(m_hMainWnd, IDR_TITLE);
	if (XC_IsShape(hTitleText))
	{
		XShapeText_SetFont(hTitleText, hFont);
	}
}
void CMainFrame::CreateMenu()
{
	//设置菜单
	HELE hMenu = (HELE)XC_GetObjectByID(m_hMainWnd, IDC_MENU_BAR);
	if (!XC_IsHELE(hMenu))
	{
		return;
	}
	
	XMenuBar_AddButton(hMenu, L"文件");
	XMenuBar_AddButton(hMenu, L"编辑");

	HMENUX hFileMenu = XMenuBar_GetMenu(hMenu, 0);
	if (hFileMenu)
	{
		XMenu_AddItem(hFileMenu, LOAD_MODULE, L"加载模板");
		XMenu_AddItem(hFileMenu, SAVE_MODULE, L"保存模板");
		XMenu_AddItem(hFileMenu, 0, L"",XC_ID_ROOT,menu_item_flags_separator); //分隔栏
		XMenu_AddItem(hFileMenu, EXIT_PROCESS, L"退出程序");
	}

	HMENUX hModifyMenu = XMenuBar_GetMenu(hMenu, 1);
	if (hModifyMenu)
	{
		XMenu_AddItem(hModifyMenu, MODIFY_MODULE, L"编辑模板");
	}

	XEle_RegEventCPP(hMenu, XE_MENU_SELECT, &CMainFrame::OnMenuSelect);
}

int CMainFrame::OnMenuSelect(int nItem, BOOL *pbHandled)
{
	*pbHandled = TRUE;
	switch (nItem)
	{
	case EXIT_PROCESS:
		{
			XWnd_CloseWindow(m_hMainWnd);
		}
		break;
	case LOAD_MODULE:
		{
			LoadPolicy();
		}
		break;
	case MODIFY_MODULE:
		{
			CLoadModuleWnd loadModuleWnd;
			loadModuleWnd.Init();
		}
	default:
		break;
	}

	return 0;
}

void CMainFrame::CreateSysButton()
{
	//系统按钮
	HXCGUI hMinBtn = XC_GetObjectByID(m_hMainWnd, IDC_MIN_BTN);
	HXCGUI hMaxBtn = XC_GetObjectByID(m_hMainWnd, IDC_MAX_BTN);
	HXCGUI hCloseBtn = XC_GetObjectByID(m_hMainWnd, IDC_CLOSE_BTN);

	if (XC_IsHELE(hMinBtn) &&
		XC_IsHELE(hMaxBtn) &&
		XC_IsHELE(hCloseBtn))
	{
		XBtn_AddBkImage((HELE)hMinBtn, button_state_leave, XImage_LoadFile(L"../SysBtn/MinNormal.bmp"));
		XBtn_AddBkImage((HELE)hMinBtn, button_state_stay, XImage_LoadFile(L"../SysBtn/MinNormal.bmp"));
		XBtn_AddBkImage((HELE)hMinBtn, button_state_down, XImage_LoadFile(L"../SysBtn/MinFocus.bmp"));

		XBtn_AddBkImage((HELE)hMaxBtn, button_state_leave, XImage_LoadFile(L"../SysBtn/MaxNormal.bmp"));
		XBtn_AddBkImage((HELE)hMaxBtn, button_state_stay, XImage_LoadFile(L"../SysBtn/MaxNormal.bmp"));
		XBtn_AddBkImage((HELE)hMaxBtn, button_state_down, XImage_LoadFile(L"../SysBtn/MaxFocus.bmp"));

		XBtn_AddBkImage((HELE)hCloseBtn, button_state_leave, XImage_LoadFile(L"../SysBtn/CloseNormal.bmp"));
		XBtn_AddBkImage((HELE)hCloseBtn, button_state_stay, XImage_LoadFile(L"../SysBtn/CloseNormal.bmp"));
		XBtn_AddBkImage((HELE)hCloseBtn, button_state_down, XImage_LoadFile(L"../SysBtn/CloseFocus.bmp"));
	}
}

void CMainFrame::CreateOutputView()
{
	HELE hOutputView = (HELE)XC_GetObjectByID(m_hMainWnd, IDC_OUTPUT_EDIT);
	if (XC_IsHELE(hOutputView))
	{
		XRichEdit_EnableReadOnly(hOutputView, TRUE); //启用只读模式
		XRichEdit_EnableMultiLine(hOutputView, TRUE); //启用多行模式
		XRichEdit_EnableAutoWrap(hOutputView, TRUE); //启用自动换行
	}

	HELE hCombo = (HELE)XC_GetObjectByID(m_hMainWnd, IDC_COMBO_BOX);
	if (!XC_IsHELE(hCombo))
	{
		return;
	}

	XComboBox_EnableEdit(hCombo, FALSE);
}

void CMainFrame::LoadPolicy()
{
	HELE hCombo = (HELE)XC_GetObjectByID(m_hMainWnd, IDC_COMBO_BOX);
	if (!XC_IsHELE(hCombo))
	{
		return;
	}

	HXCGUI hAdapter = XComboBox_GetAdapter(hCombo);
	if(NULL == hAdapter)
	{
		hAdapter = XAdTable_Create();
		XComboBox_BindAdapter(hCombo, hAdapter);
		XAdTable_AddColumn(hAdapter, L"name");
	}

	XAdTable_DeleteItemAll(hAdapter);

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

	while (TRUE)
	{
		TCHAR szPolicyName[MAX_PATH] = _T("");
		StringCchCopy(szPolicyName, MAX_PATH, wfd.cFileName);
		size_t sNameLength = 0;
		StringCchLength(szPolicyName, MAX_PATH, &sNameLength);
		szPolicyName[sNameLength - 7] = _T('\0');
		
		XAdTable_AddItemText(hAdapter, szPolicyName);
		if(!FindNextFile(hFind, &wfd))
		{
			break;
		}
	}
}

int CMainFrame::OnBtnClick(BOOL *pbHandled)
{
	HELE hUrlEdit = (HELE)XC_GetObjectByID(m_hMainWnd, IDE_URL_EDIT);
	if (XC_IsHELE(hUrlEdit))
	{
		TCHAR szUrl[MAX_PATH] = _T("");
		XRichEdit_GetText(hUrlEdit, szUrl, MAX_PATH);
		
		LPPYTHON_MODULES_DATA pTemp = m_PyManager.GetFirstScript();
		while (NULL != pTemp)
		{
			TCHAR szHasVulUrl[MAX_PATH] = _T("");
			if (m_PyManager.CallScanMethod(pTemp, szUrl, _T("GET"), szHasVulUrl, MAX_PATH))
			{
				g_OutputString(_T("在[%s]处发现弱点:%s!!!!!!!!!"), szHasVulUrl, pTemp->szVulName);
			}

			pTemp = pTemp->NextModule;
		}
	}
	*pbHandled = TRUE;
	return 0;
}