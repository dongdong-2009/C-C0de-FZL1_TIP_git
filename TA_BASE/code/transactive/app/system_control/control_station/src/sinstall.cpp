/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/sinstall.cpp $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */




#include "stdafx.h"
#include "resource.h"
#include "sinstall.h"

#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"

using namespace TA_Base_Core;

ScreenSaverInstall::ScreenSaverInstall()
{
	
}

//*****************************************************************************
//-----------------------------------------------------------------------------
BOOL ScreenSaverInstall::InitInstance()
{
	// only a single instance is allowed
	ASSERT(::AfxGetAppName() != NULL);
	VERIFY(::CreateMutex(NULL, FALSE, ::AfxGetAppName()) != NULL);
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}
    
	// install it
	if (!this->InstallIt() )
	{
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
			"falied in installing the transactive screensaver ");
		return FALSE;
	}

    return TRUE;
}
//-----------------------------------------------------------------------------
BOOL ScreenSaverInstall::InstallIt() const
{
	CWaitCursor curWait;

	// construct the full path name of the screen saver
	CString strSavePath;
	VERIFY(::GetWindowsDirectory(strSavePath.GetBuffer(MAX_PATH), MAX_PATH) != 0);
	strSavePath.ReleaseBuffer();

	if (strSavePath.Right(1) != _T("\\"))
	{
		strSavePath += _T('\\');
	}
	
	strSavePath += CString(reinterpret_cast<LPCTSTR>(IDS_SCRFILENAME));

	// Get the application's path
    char    szAppPath[MAX_PATH] = "";
    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	CString strSourcePath = szAppPath;
	strSourcePath = strSourcePath.Left(strSourcePath.ReverseFind('\\'));
	
	if (strSourcePath.Right(1) != _T("\\"))
	{
		strSourcePath += _T('\\');
	}
	
	strSourcePath += CString(reinterpret_cast<LPCTSTR>(IDS_SCRFILENAME));
	CFileFind fileFind;
	
	if (! fileFind.FindFile(strSavePath))
	{
		BOOL bCopy = CopyFile(strSourcePath, strSavePath, FALSE);
		if (!bCopy) return FALSE;
	}
	// install the screen saver and select it as default
	return this->SelectAsDefault(strSavePath);//this->InstallFile(strSavePath, MAKEINTRESOURCE(ID_RESSAVER)) &&
}


BOOL ScreenSaverInstall::SelectAsDefault(LPCTSTR lpszScr) const 
{
	ASSERT(::AfxIsValidString(lpszScr));

	CString strShortScr;
	return ::GetShortPathName(lpszScr,
							  strShortScr.GetBuffer(MAX_PATH), MAX_PATH)
			&&
			::WritePrivateProfileString(_T("boot"), _T("SCRNSAVE.EXE"),
										strShortScr, _T("system.ini"))
			&&
			::SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, TRUE);
}



