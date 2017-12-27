/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/PowerDemandEditor/src/PowerDemandEditor.cpp $
  * @author:  wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the base application class for the power demand editor.  It is implemented
  * as a managed process.
  */

#include "app/scada/PowerDemandEditor/src/stdafx.h"

#include "app/scada/PowerDemandEditor/src/PowerDemandEditor.h"
#include "app/scada/PowerDemandEditor/src/PowerDemandEditorDlg.h"
#include "app/scada/PowerDemandEditor/src/PowerDemandEditorGUI.h"


#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CPowerDemandEditorApp

	BEGIN_MESSAGE_MAP(CPowerDemandEditorApp, TransActiveWinApp)
		//{{AFX_MSG_MAP(CPowerDemandEditorApp)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CPowerDemandEditorApp construction

	CPowerDemandEditorApp::CPowerDemandEditorApp() : TransActiveWinApp( new PowerDemandEditorGUI, "Power Demand Editor" )
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	// The one and only CPowerDemandEditorApp object

	CPowerDemandEditorApp theApp;
}

