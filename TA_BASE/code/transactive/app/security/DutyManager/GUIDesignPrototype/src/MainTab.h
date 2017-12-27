/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/MainTab.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_MAINTAB_H__850DF07A_A970_4EAC_8C72_C6329693E000__INCLUDED_)
#define AFX_MAINTAB_H__850DF07A_A970_4EAC_8C72_C6329693E000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus\mfc_extensions\src\mfc_extensions.h"
#include "SiteSummaryDialog.h"
#include "SubSystemSummaryDialog.h"

// MainTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMainTab window

class CMainTab : public CTabDialogCtrl
{
// Construction
public:
	CMainTab();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainTab)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainTab();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainTab)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	CSiteSummaryDialog         m_siteSummaryDialog;
	CSubSystemSummaryDialog    m_subSystemSummaryDialog;


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINTAB_H__850DF07A_A970_4EAC_8C72_C6329693E000__INCLUDED_)
