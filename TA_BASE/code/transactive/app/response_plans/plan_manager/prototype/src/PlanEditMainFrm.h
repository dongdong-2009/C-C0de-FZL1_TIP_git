/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanEditMainFrm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanEditMainFrm.h

#if !defined(AFX_PLANEDITMAINFRM_H__24FA27D8_F5A7_11D7_B700_0050BAB1B89C__INCLUDED_)
#define AFX_PLANEDITMAINFRM_H__24FA27D8_F5A7_11D7_B700_0050BAB1B89C__INCLUDED_

class CPlanEditMainFrame : public CFrameWnd
{
protected: // create from serialization only
    CPlanEditMainFrame();
    DECLARE_DYNCREATE(CPlanEditMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanEditMainFrame)
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanEditMainFrame();

protected:

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CToolBar    m_editToolBar;
    friend class CPlanForm;

// Generated message map functions
protected:
    // {{AFX_MSG(CPlanEditMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // AFX_PLANEDITMAINFRM_H__24FA27D8_F5A7_11D7_B700_0050BAB1B89C__INCLUDED_
