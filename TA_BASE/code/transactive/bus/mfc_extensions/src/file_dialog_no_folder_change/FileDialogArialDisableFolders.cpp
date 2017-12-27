/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/file_dialog_no_folder_change/FileDialogArialDisableFolders.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  */
//////////////////////////////////////////////////////////////////////////////////////////////////

// Customized version of the File Open common dialog
// Written by Shanker.C
// Friday, April 30, 1999
// Bug note: The program will NOT remove folders from the shell namespace's root

//////////////////////////////////////////////////////////////////////////////////////////////////


#include "FileDialogArialDisableFolders.h"
namespace TA_Base_Bus
{

	//////////////////////////////////////////////////////////////////////////////////////////////////

	BEGIN_MESSAGE_MAP(FileDialogArialDisableFolders, CFileDialogArial)
	//{{AFX_MSG_MAP(FileDialogArialDisableFolders)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	HHOOK HookHandleFD; //extern 
	int FileDialogArialDisableFolders::OnceOnly = 0;

	//////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL FileDialogArialDisableFolders::OnInitDialog()
	{
		// Hide unwanted controls
		EnumChildWindows((HWND)::GetParent(this -> m_hWnd), EnumChildProc, (LPARAM)NULL);
		// Set hook
		HookHandleFD = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC) Hooker, 
			(HINSTANCE)NULL, (DWORD)GetCurrentThreadId());
		
		return CFileDialogArial::OnInitDialog();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	FileDialogArialDisableFolders::~FileDialogArialDisableFolders() 
	{
		UnhookWindowsHookEx(HookHandleFD);
		OnceOnly = 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	LRESULT CALLBACK Hooker(int nCode, WPARAM wParam,  LPARAM lParam)
	{
		TCHAR szClassName[MAX_CHAR];
		TCHAR szItemName [MAX_CHAR];
		TCHAR szEditBuff [MAX_CHAR];
		LVITEM item;
		static HWND hWndToolbar = NULL; // Persist
			
		CWPSTRUCT* x = (CWPSTRUCT*)lParam;
		GetClassName(x -> hwnd, szClassName, MAX_CHAR);
		
		if(strcmp(_strlwr(szClassName), "toolbarwindow32") == 0)
		{
			if(!FileDialogArialDisableFolders::OnceOnly) // Save toolbar's handle only once
			{
				hWndToolbar = x -> hwnd;
				++FileDialogArialDisableFolders::OnceOnly;
			}
		}
		
		if(strcmp(_strlwr(szClassName), "syslistview32") == 0) 
		{
			switch(x -> message)
			{
				
			case WM_NCPAINT : // Needed if there are no items to display
			case LAST_LISTVIEW_MSG : // Magic message sent after all items are inserted
			
				int count = ListView_GetItemCount(x -> hwnd);
					
				for(int i = 0; i < count; i++)
				{
					item.mask	= LVIF_TEXT | LVIF_PARAM;
					item.iItem = i;
					item.iSubItem = 0;
					item.pszText = szItemName;   
					item.cchTextMax = MAX_CHAR;
					ListView_GetItem(x -> hwnd, &item);
					int val = GetFileAttributes(szItemName);
					if(val == FILE_ATTRIBUTE_DIRECTORY) 
						 ListView_DeleteItem(x -> hwnd, i);
					break; 
				}
			} // end switch 
			HideToolbarBtns(hWndToolbar);
		} // end if

		if(strcmp(_strlwr(szClassName), "edit") == 0) 
		{
			switch(x -> message)
			{
				case EDIT_ENTER:	// User presses Enter
				
					::GetWindowText(x -> hwnd, szEditBuff, MAX_CHAR);
					
					if(ParseForDelims(szEditBuff))
						::SetWindowText(x -> hwnd, "");
					break;

			} // end switch

		}	// end if

		return CallNextHookEx(HookHandleFD, nCode, wParam, lParam);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL ParseForDelims(const TCHAR* szEditBuff)
	{
		for(int i = 0; i < (int)strlen(szEditBuff); ++i)
			if(szEditBuff[i] == '\\' || szEditBuff[i] == ':')
				return TRUE;
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
	{
		int id = ::GetDlgCtrlID(hWnd);
		
		switch(id)
		{
			case LOOK_IN_COMBO : // Combo box on top of the dialog
			case VERTICAL_TOOL_BAR:

				::EnableWindow(hWnd, FALSE);
				break;
		}
		
	  return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void HideToolbarBtns(HWND hWndToolbar)
	{
		TBBUTTONINFO tbinfo;
		tbinfo.cbSize = sizeof(TBBUTTONINFO);
		tbinfo.dwMask = TBIF_STATE;
		tbinfo.fsState = TBSTATE_HIDDEN | TBSTATE_INDETERMINATE;
		::SendMessage(hWndToolbar,TB_SETBUTTONINFO,(WPARAM)TB_BTN_UPONELEVEL,(LPARAM)&tbinfo);
		::SendMessage(hWndToolbar,TB_SETBUTTONINFO,(WPARAM)TB_BTN_NEWFOLDER,(LPARAM)&tbinfo);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

}