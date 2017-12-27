/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/banner/src/banner.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * The class, BannerApp, provides the CWinApp for this MFC project.
  * The app uses the GenericGUI framework to display a top level dialog of type BannerDlg.
  * Refer to the BannerDlg for more information 
  *
  */

#if !defined(AFX_BANNER_H__8457DD48_0374_4DA0_BA32_112A601E1D5E__INCLUDED_)
#define AFX_BANNER_H__8457DD48_0374_4DA0_BA32_112A601E1D5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/system_control/banner/src/resource.h"		// main symbols
#include "bus\generic_gui\src\TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// BannerApp:
// See banner.cpp for the implementation of this class
//

class BannerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	BannerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BannerApp)
	public:
   	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL


// Implementation

	//{{AFX_MSG(BannerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
            
	BOOL InitATL();

	BOOL m_bATLInited;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BANNER_H__8457DD48_0374_4DA0_BA32_112A601E1D5E__INCLUDED_)
