#pragma once
#include "stdafx.h"

#define IDR_TITLE 105
#define IDC_MIN_BTN 106
#define IDC_CLOSE_BTN 108
#define IDC_TREE 109

BOOL UTF8_2_UNICODE(LPTSTR lpBuff, DWORD dwBuffSize, LPCSTR lpUTF8Str);

class CLoadModuleWnd
{
public:
	CLoadModuleWnd(void);
	~CLoadModuleWnd(void);
	void Init();
	void LoadModuleInfo();
	void LoadVuls(HXCGUI hAdapter, int nNote, LPTSTR lpszFile);
	int OnTreeTemplateCreateEnd(tree_item_i* pItem,BOOL *pbHandled);
	
protected:
	HWINDOW m_hWnd;
	LPPOLICY_INFO m_pHead;
	HTEMP m_hGroupTemp;
};
