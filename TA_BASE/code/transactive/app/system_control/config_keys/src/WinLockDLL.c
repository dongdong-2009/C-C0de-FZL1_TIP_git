/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/config_keys/src/WinLockDLL.c $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */


/***********************************************************************
 * WinLockDLL - Library containing functions to Lock Desktop, Taskbar, *
 *              and disable Ctrl+Alt+Del and task switching keys.      *
 ***********************************************************************/

#define     WIN32_LEAN_AND_MEAN
#define     _WIN32_WINNT 0x0400

#include    <windows.h>
#include    <stdlib.h>
#include    "winlockdll.h"
#include    "inject.h"
//TD14928 bgn
#define IMPORT_EXTENSIONS
#include	"bus\mfc_extensions\src\prnScreen\prnScreen.h"
//TD14928 end
#define     PROGRAM_MANAGER "Program Manager"	// Program manager window name
#define     TASKBAR         "Shell_TrayWnd"		// Taskbar class name
#define     ID_STARTBUTTON  0x130				// Start button ID
#define     ID_TRAY         0x12F				// System tray ID
#define     ID_CLOCK        0x12F				// System clock ID

HHOOK		hHook;          // Mouse hook
HINSTANCE	hInst;		    // Instance handle



/*************************
 * Mouse Hook procedure. *
 * (Win 9x)              *
 *************************/
LRESULT CALLBACK MouseHookProc(int nCode, WORD wParam, DWORD lParam) 
{
	if(nCode >= 0)
	{
        if (wParam == WM_LBUTTONDBLCLK)
        {
            if (((MOUSEHOOKSTRUCT *)lParam)->hwnd == GetDesktopWindow())
            {
                return 1;
            }
        }
	}

    return CallNextHookEx(hHook, nCode, wParam, lParam); 
} 


/***********************************
 * Low Level Mouse Hook procedure. *
 * (Win NT4SP3+)                   *
 ***********************************/
LRESULT CALLBACK LowLevelMouseHookProc(int nCode, WORD wParam, DWORD lParam) 
{
	PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;
	HWND hWnd = WindowFromPoint(p->pt);

	if(nCode >= 0)
	{
        if ((wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) && hWnd == GetDesktopWindow())
        {
			return 1;
        }
	}

    return CallNextHookEx(hHook, nCode, wParam, lParam); 
} 


/*************************
 * Hide/Show Desktop.    *
 * TRUE=Show, FALSE=Hide * 
 * (Win9x/NT/2K/XP)      *
 *************************/

int DLL_EXP_IMP WINAPI Desktop_Show_Hide(BOOL bShowHide)
{
	OSVERSIONINFO	osvi;
	BOOL			bIsWindowsNT4SP3orLater;
	int				iServicePack;
	char           *p;

	// Determine the current windows version
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	for (p = osvi.szCSDVersion; *p  && !isdigit(*p); *p++);
	iServicePack = atoi(p); 
	bIsWindowsNT4SP3orLater = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) && 
		                      (((osvi.dwMajorVersion == 4) && (iServicePack >= 3)) || 
						      (osvi.dwMajorVersion > 4));

    if (!bShowHide)
    {
        if (!hHook)
		{
            hHook  = SetWindowsHookEx(bIsWindowsNT4SP3orLater ? WH_MOUSE_LL : WH_MOUSE, 
									  bIsWindowsNT4SP3orLater ? (HOOKPROC)LowLevelMouseHookProc : (HOOKPROC)MouseHookProc, 
                                      hInst, 
                                      0);
			if (!hHook)
				return 0;
		}
    }
    else
    {
        UnhookWindowsHookEx(hHook);
        hHook = NULL;
    }

    return SetWindowPos(FindWindow(NULL, PROGRAM_MANAGER),
                        NULL,
                        0, 0, 0, 0,
                        bShowHide ? SWP_SHOWWINDOW : SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
}


/***************************
 * Hide/Show Start Button. *
 * TRUE=Show, FALSE=Hide   * 
 * (Win9x/NT/2K/XP)        *
 ***************************/
int DLL_EXP_IMP WINAPI StartButton_Show_Hide(BOOL bShowHide)
{
    HWND    hWnd;

    hWnd = GetDlgItem(FindWindow(TASKBAR, NULL), ID_STARTBUTTON);
    if (hWnd == NULL)
        return 0;

    ShowWindow(hWnd, bShowHide ? SW_SHOW : SW_HIDE);
    UpdateWindow(hWnd);

    return 1;
}


/*************************
 * Hide/Show Taskbar.    *
 * TRUE=Show, FALSE=Hide * 
 * (Win9x/NT/2K/XP)      *
 *************************/
int DLL_EXP_IMP WINAPI Taskbar_Show_Hide(BOOL bShowHide)
{
    HWND    hWnd;

    hWnd = FindWindow(TASKBAR, NULL);
    if (hWnd == NULL)
        return 0;

    ShowWindow(hWnd, bShowHide ? SW_SHOW : SW_HIDE);
    UpdateWindow(hWnd);

    return 1;
}


/***************************
 * Hide/Show System Clock. *
 * TRUE=Show, FALSE=Hide   * 
 * (Win9x/NT/2K/XP)        *
 ***************************/
int DLL_EXP_IMP WINAPI  Clock_Show_Hide(BOOL bShowHide)
{
    HWND    hWnd;

    hWnd = GetDlgItem(FindWindow(TASKBAR, NULL), ID_TRAY);
	hWnd = GetDlgItem(hWnd, ID_CLOCK);
    if (hWnd == NULL)
        return 0;

    ShowWindow(hWnd, bShowHide ? SW_SHOW : SW_HIDE);
    UpdateWindow(hWnd);

    return 1;
}


/************************************************************
 * Enable/Disable special keys.                             *
 * (Ctrl+Alt+Del, Alt+Tab, Alt+Esc, Ctrl+Esc, Win key, ...) *
 * TRUE=Enable, FALSE=Disable                               * 
 * (Win 9x)                                                 *
 ************************************************************/
int DLL_EXP_IMP WINAPI  Keys_Enable_Disable(BOOL bEnableDisable)
{
    BOOL    bOldState;

    return SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, bEnableDisable ? FALSE : TRUE, &bOldState, 0);
}


/************************************
 * Enable/Disable Alt+Tab, Alt+Esc. *
 * TRUE=Enable, FALSE=Disable       * 
 * ( Win ?)                         *
 ************************************/
int DLL_EXP_IMP WINAPI AltTab1_Enable_Disable(BOOL bEnableDisable)
{
    BOOL    bOldState;

    return SystemParametersInfo(SPI_SETFASTTASKSWITCH, bEnableDisable ? FALSE : TRUE, &bOldState, 0);
}


/************************************
 * Enable/Disable Alt+Tab, Alt+Esc. *
 * TRUE=Enable, FALSE=Disable       * 
 * (Win 2K)                         *
 ************************************/
int DLL_EXP_IMP WINAPI AltTab2_Enable_Disable(HWND hWnd, BOOL bEnableDisable)
{
    #define m_nHotKeyID 100

    if (!bEnableDisable)
	{
		if (!RegisterHotKey(hWnd, m_nHotKeyID+0, MOD_ALT, VK_TAB))		// Alt+Tab
			return 0;
        if (!RegisterHotKey(hWnd, m_nHotKeyID+1, MOD_ALT, VK_ESCAPE))	// Alt+Esc
			return 0;
    }
	else
	{
        if (!UnregisterHotKey(hWnd, m_nHotKeyID+0))
			return 0;
        if (!UnregisterHotKey(hWnd, m_nHotKeyID+1))
			return 0;
	}
	return 1;
}


/**************************************
 * Low Level Keyboard Hook procedure. *
 * (Win NT4SP3+)                      *
 **************************************/
//TD14928 bgn
HHOOK hPrnHook = NULL;  // Old low level keyboard hook 

BOOL prns1 = FALSE, prns2 = FALSE, prns3 = FALSE; //may be use some lock will be better
LRESULT CALLBACK PrnKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    PKBDLLHOOKSTRUCT p;
	
    if (nCode == HC_ACTION) 
    {
        p = (PKBDLLHOOKSTRUCT) lParam;
		
		if(p->vkCode == '1' && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0))
		{
			if(!prns1)
			{
				prns1 = TRUE;
				CapturePrintScreen(0);
				prns1 = FALSE;
			}
			return 1;
		}
		else if(p->vkCode == '2' && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0))
		{
			if(!prns2)
			{
				prns2 = TRUE;
				CapturePrintScreen(1280);
				prns2 =FALSE;
			}
			return 1;
		}
		else if(p->vkCode == '3' && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0))
		{
			if(!prns3)
			{
				prns3 = TRUE;
				CapturePrintScreen(2560);
				prns3 =FALSE;
			}
			return 1;
		}
		else if(p->vkCode == '4' && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0))
		{
			return 1;
		}
	}
	
	return CallNextHookEx(hPrnHook, nCode, wParam, lParam);
}

/*****************
Hook for print
*******************/
int DLL_EXP_IMP WINAPI Prn_Enable_Disable(BOOL bEnableDisable)
{
    if (!bEnableDisable)
    {
        if (!hPrnHook)
		{
            // Install the low-level keyboard hook
            hPrnHook  = SetWindowsHookEx(WH_KEYBOARD_LL, 
				PrnKeyboardProc, 
				hInst, 
				0);
			if (!hPrnHook)
				return 0;
		}
    }
    else
    {
        UnhookWindowsHookEx(hPrnHook);
        hPrnHook = NULL;
    }
	
    return 1;
}
//TD14928 end
HHOOK hKeyboardHook = NULL;  // Old low level keyboard hook 

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    PKBDLLHOOKSTRUCT p;

    if (nCode == HC_ACTION) 
    {
        p = (PKBDLLHOOKSTRUCT) lParam;
		if (
            // WIN key (for Start Menu)
            ((p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN)) ||       
            // ALT+TAB
            (p->vkCode == VK_TAB && p->flags & LLKHF_ALTDOWN) ||       
            // ALT+ESC
            (p->vkCode == VK_ESCAPE && p->flags & LLKHF_ALTDOWN) ||    
            // CTRL+ESC
            ((p->vkCode == VK_ESCAPE) && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)) ||
            // CTRL+SHIFT+ESC
            ((p->vkCode == VK_ESCAPE) && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) && ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)) ||
			// CTRL+ALT+DEL (Unfortunately doesn't work !)
            ((p->vkCode == VK_DELETE) && ( (p->flags & LLKHF_ALTDOWN) != 0 ) && ( (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0))
            )
            return 1;
   }
                    
   return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}



/*****************************************************
 * Enable/Disable task switching keys.               *
 * (Alt+Tab, Alt+Esc, Ctrl+Esc, Win, Ctrl+Shift+Esc) *
 * TRUE=Enable, FALSE=Disable                        *
 * (Win NT4SP3+, Win2K).                             *
 *****************************************************/
int DLL_EXP_IMP WINAPI TaskSwitching_Enable_Disable(BOOL bEnableDisable)
{
    if (!bEnableDisable)
    {
        if (!hKeyboardHook)
		{
            // Install the low-level keyboard hook
            hKeyboardHook  = SetWindowsHookEx(WH_KEYBOARD_LL, 
                                              LowLevelKeyboardProc, 
                                              hInst, 
                                              0);
			if (!hKeyboardHook)
				return 0;
		}
    }
    else
    {
        UnhookWindowsHookEx(hKeyboardHook);
        hKeyboardHook = NULL;
    }

    return 1;
}


/****************************************
 * Disable Task Manager (CTRL+ALT+DEL). *
 * TRUE=Enable, FALSE=Disable           *
 * (Win NT/2K/XP)                       *
 ****************************************/
int DLL_EXP_IMP WINAPI TaskManager_Enable_Disable(BOOL bEnableDisable)
{
    #define KEY_DISABLETASKMGR		"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"
    #define VAL_DISABLETASKMGR		"DisableTaskMgr"
	#define VAL_DISABLETCHANGEPWD	"DisableChangePassword"
	#define VAL_DISABLETLOCK		"DisableLockWorkstation"

    HKEY    hKey;
    DWORD   val;
	LONG	r[3];

    if (RegOpenKey(HKEY_CURRENT_USER, KEY_DISABLETASKMGR, &hKey) != ERROR_SUCCESS)
        if (RegCreateKey(HKEY_CURRENT_USER, KEY_DISABLETASKMGR, &hKey) != ERROR_SUCCESS)
			return 0;

    if (bEnableDisable) // Enable
    {
        r[0] = RegDeleteValue(hKey, VAL_DISABLETASKMGR);
		r[1] = RegDeleteValue(hKey, VAL_DISABLETCHANGEPWD);
		r[2] = RegDeleteValue(hKey, VAL_DISABLETLOCK);
    }
    else                // Disable
    {
        val = 1;
        r[0] = RegSetValueEx(hKey, VAL_DISABLETASKMGR, 0, REG_DWORD, (BYTE *)&val, sizeof(val));
		r[1] = RegSetValueEx(hKey, VAL_DISABLETCHANGEPWD, 0, REG_DWORD, (BYTE *)&val, sizeof(val));
		r[2] = RegSetValueEx(hKey, VAL_DISABLETLOCK, 0, REG_DWORD, (BYTE *)&val, sizeof(val));
    }

    RegCloseKey(hKey);

    return ((r[0] == ERROR_SUCCESS && r[1] == ERROR_SUCCESS && r[2] == ERROR_SUCCESS) ? 1 : 0);
}


/*****************************************************************
 * Enable/Disable Ctrl+Alt+Del and Ctrl+Shift+Esc key sequences. *
 * TRUE=Enable, FALSE=Disable                                    *
 * (Win 2K).                                                     *
 *****************************************************************/
int DLL_EXP_IMP WINAPI CtrlAltDel_Enable_Disable(BOOL bEnableDisable)
{
	static BOOL bInjected = FALSE;

    if (!bEnableDisable)
    {
		if (!bInjected)
		{
			bInjected = Inject();
			return bInjected;
		}
    }
    else
    {
		if (bInjected)
		{
			bInjected = !Eject();
			return !bInjected;
		}
    }

    return 0;
}


/******************************
 * Create and start a thread. *
 ******************************/
BOOL StartThread(LPTHREAD_START_ROUTINE ThreadFunc, THREAD_DATA *td)
{
	ULONG	tid;
	HANDLE	hThread;

	// Create and start thread
	hThread = CreateThread(NULL,
						   0,
						   ThreadFunc,
						   td,
						   0,
						   &tid);

	if (!hThread)
		return FALSE;

	// Wait until thread terminates
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	return TRUE;

}


/************************************************
 * Create a new Desktop and run a Thread in it. *
 * (Win NT+).                                   *
 ************************************************/
int DLL_EXP_IMP WINAPI Thread_Desktop(LPTHREAD_START_ROUTINE ThreadFunc, THREAD_DATA *td)
{
	HDESK	hOriginalThread;
	HDESK	hOriginalInput;
	HDESK	hNewDesktop;

	// Save original ...
	hOriginalThread = GetThreadDesktop(GetCurrentThreadId());
	hOriginalInput = OpenInputDesktop(0, FALSE, DESKTOP_SWITCHDESKTOP);

	// Create a new Desktop and switch to it
	hNewDesktop = CreateDesktop(td->szDesktopName, NULL, NULL, 0, GENERIC_ALL, NULL);
	SetThreadDesktop(hNewDesktop);
	SwitchDesktop(hNewDesktop);

	// Execute thread in new desktop
	td->hDesk = hNewDesktop;
	StartThread(ThreadFunc, td);

	// Restore original ...
	SwitchDesktop(hOriginalInput);
	SetThreadDesktop(hOriginalThread);

	// Close the Desktop
	CloseDesktop(hNewDesktop);

	return 0;
}



/****************
 * Run process. *
 ****************/
BOOL StartProcess(char *szDesktopName, char *szPath)
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;

    // Zero these structs
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
	si.lpTitle = szDesktopName;
	si.lpDesktop = szDesktopName;
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process
    if (!CreateProcess(NULL,    // No module name (use command line). 
                       szPath,  // Command line. 
                       NULL,    // Process handle not inheritable. 
                       NULL,    // Thread handle not inheritable. 
                       FALSE,   // Set handle inheritance to FALSE. 
                       0,       // No creation flags. 
                       NULL,    // Use parent's environment block. 
                       NULL,    // Use parent's starting directory. 
                       &si,     // Pointer to STARTUPINFO structure.
                       &pi))    // Pointer to PROCESS_INFORMATION structure.
    {
        return FALSE;
    }

	// Wait until process exits
	WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

	return TRUE;
}


/*************************************************
 * Create a new Desktop and run a Process in it. *
 * (Win NT+).                                    *
 *************************************************/
int DLL_EXP_IMP WINAPI Process_Desktop(char *szDesktopName, char *szPath)
{
	HDESK	hOriginalThread;
	HDESK	hOriginalInput;
	HDESK	hNewDesktop;

	// Save original ...
	hOriginalThread = GetThreadDesktop(GetCurrentThreadId());
	hOriginalInput = OpenInputDesktop(0, FALSE, DESKTOP_SWITCHDESKTOP);

	// Create a new Desktop and switch to it
	hNewDesktop = CreateDesktop(szDesktopName, NULL, NULL, 0, GENERIC_ALL, NULL);
	SetThreadDesktop(hNewDesktop);
	SwitchDesktop(hNewDesktop);

	// Execute process in new desktop
	StartProcess(szDesktopName, szPath);

	// Restore original ...
	SwitchDesktop(hOriginalInput);
	SetThreadDesktop(hOriginalThread);

	// Close the Desktop
	CloseDesktop(hNewDesktop);

	return 0;
}

