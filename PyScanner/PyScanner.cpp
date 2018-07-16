// PyScanner.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "PyScanner.h"
#include "MainFrame.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	XInitXCGUI(NULL);
	CMainFrame MainWnd;
	MainWnd.Init();
	XRunXCGUI();
	XExitXCGUI();
}