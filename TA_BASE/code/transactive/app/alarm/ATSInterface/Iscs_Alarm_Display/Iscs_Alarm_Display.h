// Iscs_Alarm_Display.h : main header file for the ISCS_ALARM_DISPLAY application
//

#if !defined(AFX_ISCS_ALARM_DISPLAY_H__EE07FF63_DD08_44B9_8293_6A8150AA6FCB__INCLUDED_)
#define AFX_ISCS_ALARM_DISPLAY_H__EE07FF63_DD08_44B9_8293_6A8150AA6FCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIscs_Alarm_DisplayApp:
// See Iscs_Alarm_Display.cpp for the implementation of this class
//

class CIscs_Alarm_DisplayApp : public CWinApp
{
public:
	CIscs_Alarm_DisplayApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIscs_Alarm_DisplayApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIscs_Alarm_DisplayApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISCS_ALARM_DISPLAY_H__EE07FF63_DD08_44B9_8293_6A8150AA6FCB__INCLUDED_)
