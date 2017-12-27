/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusViewer.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Main Application class
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786 4284)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "stdafx.h"
#include "EquipmentStatusViewer.h"
#include "EquipmentStatusViewerDlg.h"
#include "EquipmentStatusViewerGUI.h"

#ifdef _DEBUG   
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
	BEGIN_MESSAGE_MAP(CEquipmentStatusViewerApp, TransActiveWinApp)
		//{{AFX_MSG_MAP(CEquipmentStatusViewerApp)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG
	END_MESSAGE_MAP()


	CEquipmentStatusViewerApp::CEquipmentStatusViewerApp() : 
		TA_Base_Bus::TransActiveWinApp(new EquipmentStatusViewerGUI(), "Equipment Status Viewer")
	{
	}

	BOOL CEquipmentStatusViewerApp::InitInstance()
	{
		HWND hWnd = ::FindWindow(NULL,"Equipment Status Viewer"); 
		if (hWnd) 		
			return FALSE; 
		else
			return TransActiveWinApp::InitInstance();			
	}

	CEquipmentStatusViewerApp theApp;	
}
