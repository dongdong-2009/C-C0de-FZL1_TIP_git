/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutil.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// crutil.h : main header file for the CRUTIL application
//

#if !defined(AFX_CRUTIL_H__0EF562BC_9F0F_483D_B8C8_C23E9A912E29__INCLUDED_)
#define AFX_CRUTIL_H__0EF562BC_9F0F_483D_B8C8_C23E9A912E29__INCLUDED_

#include <dbghelp.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "CommandLineParser.hpp"
#include "CrystalReportsHelper.hpp"

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/TransActiveWinApp.h"

namespace TA_Base_App
{
//namespace TA_Reporting
//{
#endif // TRANSACTIVE_BUILD

	/////////////////////////////////////////////////////////////////////////////
	// CCrutilApp:
	// See crutil.cpp for the implementation of this class
	//

	#ifdef TRANSACTIVE_BUILD
	class CCrutilApp : public TA_Base_Bus::TransActiveWinApp
	#else
	class CCrutilApp : public CWinApp
	#endif // TRANSACTIVE_BUILD

	{
	public:
		CCrutilApp();

		#ifndef TRANSACTIVE_BUILD
		void display( CommandLineParser* p_objArgv = NULL, char* p_pszReportName = 0, CrystalReportsHelper* p_pobjCrystalReports = 0 );
		#endif // TRANSACTIVE_BUILD

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CCrutilApp)
		public:
		virtual BOOL InitInstance();
		virtual int ExitInstance();
		//}}AFX_VIRTUAL

	// Implementation

		//{{AFX_MSG(CCrutilApp)
			// NOTE - the ClassWizard will add and remove member functions here.
			//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

#ifdef TRANSACTIVE_BUILD
//} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRUTIL_H__0EF562BC_9F0F_483D_B8C8_C23E9A912E29__INCLUDED_)
