/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/file_dialog_no_folder_change/FileDialogArialDisableFolders.h $
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

//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _FILEDIALOG_ARIAL_DISABLE_FOLDERS_H_
#define _FILEDIALOG_ARIAL_DISABLE_FOLDERS_H_

//////////////////////////////////////////////////////////////////////////////////////////////////

#include <afxcmn.h>
#include <afxdlgs.h>
#include <commdlg.h>
#include <cderr.h>
#include "bus/mfc_extensions/src/standard_dialogs_arial_bold/FileDialogArial.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus {

	const int _10K_				= 10000;
	const int MAX_CHAR			= 250;
	const int TB_BTN_UPONELEVEL = 40961;
	const int TB_BTN_NEWFOLDER	= 40962;
	const int LOOK_IN_COMBO		= 1137; 
	const int VERTICAL_TOOL_BAR = 1184;
	const int LAST_LISTVIEW_MSG = 4146;
	const int EDIT_ENTER		= 14;
	const int BLOCK_GRANULARITY = 30;
	#define FILE_DIALOG_STYLE OFN_ALLOWMULTISELECT | OFN_ENABLEHOOK | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST

//////////////////////////////////////////////////////////////////////////////////////////////////

	class AFX_EXT_CLASS FileDialogArialDisableFolders : public CFileDialogArial
	{
		public:

			FileDialogArialDisableFolders(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
											LPCTSTR lpszDefExt = NULL,
											LPCTSTR lpszFileName = NULL,
											DWORD dwFlags = FILE_DIALOG_STYLE,
											LPCTSTR lpszFilter = NULL,
											CWnd* pParentWnd = NULL) : 
			CFileDialogArial (bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd) {}

			virtual ~FileDialogArialDisableFolders(); 
			static int OnceOnly;

		protected:

		//{{AFX_MSG(CFileDialogArialDiableFolders)
		virtual BOOL OnInitDialog();
		//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

		private:
	}; 

	BOOL ParseForDelims(const TCHAR* szEditBuff);
	void HideToolbarBtns(HWND);
	LRESULT CALLBACK Hooker(int, WPARAM, LPARAM);
	BOOL CALLBACK EnumChildProc(HWND, LPARAM);
	extern HHOOK HookHandleFD;

}

//////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_FILEDIALOG_ARIAL_DISABLE_FOLDERS_H_

//////////////////////////////////////////////////////////////////////////////////////////////////
