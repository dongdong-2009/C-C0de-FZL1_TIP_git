/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanControlMainFrm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanControlMainFrm.h

#if !defined(AFX_PLANCONTROLMAINFRM_H__24FA27D8_F5A7_11D7_B700_0050BAFFB89C__INCLUDED_)
#define AFX_PLANCONTROLMAINFRM_H__24FA27D8_F5A7_11D7_B700_0050BAFFB89C__INCLUDED_

class CPlanControlMainFrame : public CFrameWnd
{
protected: // create from serialization only
    CPlanControlMainFrame();
    DECLARE_DYNCREATE(CPlanControlMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanControlMainFrame)
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanControlMainFrame();

protected:

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CToolBar    m_ctrlToolBar;
    friend class CPlanForm;

// Generated message map functions
protected:
    // {{AFX_MSG(CPlanControlMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnButtonDropDown(NMTOOLBAR* pnmh, LRESULT* plRes);
    afx_msg void OnStartPlan();
    afx_msg void OnCustomisePlan();
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // AFX_PLANCONTROLMAINFRM_H__24FA27D8_F5A7_11D7_B700_0050BAFFB89C__INCLUDED_
