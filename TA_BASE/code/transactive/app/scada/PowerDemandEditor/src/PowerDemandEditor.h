/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/PowerDemandEditor/src/PowerDemandEditor.h $
  * @author:  wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the base application class for the power demand editor.  It is implemented
  * as a managed process.
  */

#ifndef POWERDEMANDEDITOR_H
#define POWERDEMANDEDITOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/scada/PowerDemandEditor/src/resource.h"		// main symbols
#include "app/scada/PowerDemandEditor/src/PowerDemandEditorGUI.h"

#include "bus/generic_gui/src/TransActiveWinApp.h"


namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CPowerDemandEditorApp:
	// See PowerDemandEditor.cpp for the implementation of this class
	//

	class CPowerDemandEditorApp : public TA_Base_Bus::TransActiveWinApp
	{
	public:
		CPowerDemandEditorApp();
		virtual ~CPowerDemandEditorApp() {};

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CPowerDemandEditorApp)
		public:
		//}}AFX_VIRTUAL

	// Implementation

		//{{AFX_MSG(CPowerDemandEditorApp)
			// NOTE - the ClassWizard will add and remove member functions here.
			//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};


	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // ifndef POWERDEMANDEDITOR_H
