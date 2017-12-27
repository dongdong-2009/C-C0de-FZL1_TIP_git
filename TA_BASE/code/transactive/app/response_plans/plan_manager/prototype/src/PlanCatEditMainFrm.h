/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanCatEditMainFrm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANCATEDITMAINFRM_H__0B91EF61_04AE_49C2_8D25_83792A2D03E6__INCLUDED_)
#define AFX_PLANCATEDITMAINFRM_H__0B91EF61_04AE_49C2_8D25_83792A2D03E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanCatEditMainFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditMainFrame frame

class CPlanCatEditMainFrame : public CFrameWnd
{
    DECLARE_DYNCREATE(CPlanCatEditMainFrame)
protected:
    CPlanCatEditMainFrame();           // protected constructor used by dynamic creation

// Attributes
protected:
    CToolBar m_toolBar;
    friend class CPlanCatForm;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanCatEditMainFrame)
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CPlanCatEditMainFrame();

    // Generated message map functions
    // {{AFX_MSG(CPlanCatEditMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANCATEDITMAINFRM_H__0B91EF61_04AE_49C2_8D25_83792A2D03E6__INCLUDED_)
