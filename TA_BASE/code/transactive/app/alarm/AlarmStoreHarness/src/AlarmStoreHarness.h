// AlarmStoreHarness.h : main header file for the ALARMSTOREHARNESS application

#if !defined(AFX_ALARMSTOREHARNESS_H__CF8D8BD4_716F_43CF_913F_43F5124C8066__INCLUDED_)
#define AFX_ALARMSTOREHARNESS_H__CF8D8BD4_716F_43CF_913F_43F5124C8066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAlarmStoreHarnessApp:
// See AlarmStoreHarness.cpp for the implementation of this class
//

class CAlarmStoreHarnessApp : public CWinApp
{
public:
	CAlarmStoreHarnessApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmStoreHarnessApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAlarmStoreHarnessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMSTOREHARNESS_H__CF8D8BD4_716F_43CF_913F_43F5124C8066__INCLUDED_)
