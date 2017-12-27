/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/config_keys/src/WinLockDll.h $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */


#ifndef WINLOCKDLL_H
#define WINLOCKDLL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef  _DLL_
#define DLL_EXP_IMP __declspec(dllexport)
#else
#define DLL_EXP_IMP __declspec(dllimport)
#endif

typedef struct _THREAD_DATA
{
	HDESK hDesk;
	char  szDesktopName[20];
} THREAD_DATA;

DLL_EXP_IMP int WINAPI  Desktop_Show_Hide(BOOL bShowHide);
DLL_EXP_IMP int WINAPI  Desktop_Show_Hide(BOOL bShowHide);
DLL_EXP_IMP int WINAPI StartButton_Show_Hide(BOOL bShowHide);
DLL_EXP_IMP int WINAPI Taskbar_Show_Hide(BOOL bShowHide);
DLL_EXP_IMP int WINAPI Clock_Show_Hide(BOOL bShowHide);
DLL_EXP_IMP int WINAPI Keys_Enable_Disable(BOOL bEnableDisable);
DLL_EXP_IMP int WINAPI AltTab1_Enable_Disable(BOOL bEnableDisable);
DLL_EXP_IMP int WINAPI AltTab2_Enable_Disable(HWND hWnd, BOOL bEnableDisable);
DLL_EXP_IMP int WINAPI TaskSwitching_Enable_Disable(BOOL bEnableDisable);
DLL_EXP_IMP int WINAPI TaskManager_Enable_Disable(BOOL bEnableDisable);
DLL_EXP_IMP int WINAPI CtrlAltDel_Enable_Disable(BOOL bEnableDisable);
DLL_EXP_IMP int WINAPI Thread_Desktop(LPTHREAD_START_ROUTINE ThreadFunc, THREAD_DATA  *td);
DLL_EXP_IMP int WINAPI Process_Desktop(char *szDesktopName, char *szPath);
DLL_EXP_IMP int WINAPI Prn_Enable_Disable(BOOL bEnableDisable);   //TD14928






#endif