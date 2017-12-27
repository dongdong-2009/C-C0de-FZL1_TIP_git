/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanStepEditMainFrm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANSTEPEDITMAINFRM_H__156E788E_F390_4278_9BA2_1C6E8A3EF34B__INCLUDED_)
#define AFX_PLANSTEPEDITMAINFRM_H__156E788E_F390_4278_9BA2_1C6E8A3EF34B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanStepEditMainFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditMainFrame frame

class CPlanStepEditMainFrame : public CFrameWnd
{
    DECLARE_DYNCREATE(CPlanStepEditMainFrame)
protected:
    CPlanStepEditMainFrame();           // protected constructor used by dynamic creation

// Attributes
protected:
    CToolBar m_toolBar;
    friend class CPlanStepForm;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanStepEditMainFrame)
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CPlanStepEditMainFrame();

    // Generated message map functions
    // {{AFX_MSG(CPlanStepEditMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANSTEPEDITMAINFRM_H__156E788E_F390_4278_9BA2_1C6E8A3EF34B__INCLUDED_)
