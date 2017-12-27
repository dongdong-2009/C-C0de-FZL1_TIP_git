/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/AlarmsGui.h $
  * @author:  A. Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * main header file for the ALARMSGUI application  
  */

#if !defined(AFX_ALARMSGUI_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)
#define AFX_ALARMSGUI_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/alarm/alarm_manager/src/resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiApp:
// See AlarmsGui.cpp for the implementation of this class
//

class CAlarmsGuiApp : public TA_Base_Bus::TransActiveWinApp
{
public:

	/**
	  * CAlarmsGuiApp
	  *
	  * Constructor for the CAlarmsGuiApp object. 
	  * 
	  * @return 
	  *
	  * @exception 
	  */
	CAlarmsGuiApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmsGuiApp)
	public:
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAlarmsGuiApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL InitInstance();
};


////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMSGUI_H__F9076B65_5412_11D5_B5EE_0080AD74DCEE__INCLUDED_)

