/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanForm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANFORM_H__E39DC7E5_A802_11D5_B63F_0050BAB1D92B__INCLUDED_)
#define AFX_PLANFORM_H__E39DC7E5_A802_11D5_B63F_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanForm.h : header file
//

#include "PvCommonDefs.h"


/////////////////////////////////////////////////////////////////////////////
// CPlanForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPlanForm : public CFormView
{
protected:
    CPlanForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CPlanForm)

// Form Data
public:
    // {{AFX_DATA(CPlanForm)
    enum { IDD = IDD_PLAN_FORM };
    CTabCtrl    m_planEditTabCtrl;
    // }}AFX_DATA

// Attributes
public:

protected:
    int m_selectedInstance;
    PV::Plan *m_plan;
    bool m_readOnly;
    bool m_controlMode;

    typedef std::vector<CDialog *> TabItemList;
    TabItemList m_tabItems;

// Operations
public:
    void PopulateData(PV::Plan *plan);
    void EnableEditing();
    void EnableControl();
    void Resize();
    void SetSelectedInstance(int si) { m_selectedInstance = si;   }
    int GetSelectedInstance() const  { return m_selectedInstance; }

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanForm)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CPlanForm)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSelchangePlaneditTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnStepEdit();
    afx_msg void OnStepCustomise();
    afx_msg void OnPlanExit();
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANFORM_H__E39DC7E5_A802_11D5_B63F_0050BAB1D92B__INCLUDED_)
