// security_simulator.h : main header file for the SECURITY_SIMULATOR application
//

#if !defined(AFX_SECURITY_SIMULATOR_H__510A879B_4D05_46FB_BFCF_DD307AFBC044__INCLUDED_)
#define AFX_SECURITY_SIMULATOR_H__510A879B_4D05_46FB_BFCF_DD307AFBC044__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSecurity_simulatorApp:
// See security_simulator.cpp for the implementation of this class
//

class CSecurity_simulatorApp : public CWinApp
{
public:
	CSecurity_simulatorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecurity_simulatorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSecurity_simulatorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECURITY_SIMULATOR_H__510A879B_4D05_46FB_BFCF_DD307AFBC044__INCLUDED_)
