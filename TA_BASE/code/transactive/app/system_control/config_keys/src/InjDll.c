/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/config_keys/src/InjDll.c $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include  <windows.h>

HWND	hSASWnd;
WNDPROC	OldSASProc;

//////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SASWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_HOTKEY)
	{
		// Ctrl+Alt+Del
		if (lParam == MAKELONG(MOD_CONTROL | MOD_ALT, VK_DELETE))
			return 1;

		// Ctrl+Shift+Esc
		if (lParam == MAKELONG(MOD_CONTROL | MOD_SHIFT, VK_ESCAPE))
			return 1;
	}

	return CallWindowProc(OldSASProc, hWnd, uMsg, wParam, lParam);
}
extern HINSTANCE	hInst;		    // Instance handle    //TD14928
///////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
	hInst = (HINSTANCE)hInstDll;    //TD14928
	switch(fdwReason)
	{
    case DLL_PROCESS_ATTACH:
		 hSASWnd = FindWindow(TEXT("SAS Window class"), TEXT("SAS window"));

		 if (hSASWnd)
			 OldSASProc = (WNDPROC)SetWindowLong(hSASWnd, GWL_WNDPROC, (LONG)SASWindowProc);
		 break;

    case DLL_PROCESS_DETACH:
         if (hSASWnd)
			 SetWindowLong(hSASWnd, GWL_WNDPROC, (LONG)OldSASProc);
		 break;
	}
    
	return TRUE;
}
