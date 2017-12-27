/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanStepForm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANSTEPFORM_H__9CBBF271_BC7C_11D5_B646_0050BAB1D92B__INCLUDED_)
#define AFX_PLANSTEPFORM_H__9CBBF271_BC7C_11D5_B646_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanStepForm.h : header file
//
#include "PvCommonDefs.h"
#include "Notes.h"


/////////////////////////////////////////////////////////////////////////////
// CPlanStepForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPlanStepForm : public CFormView
{
protected:
    CPlanStepForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CPlanStepForm)

// Form Data
public:
    // {{AFX_DATA(CPlanStepForm)
    enum { IDD = IDD_PLANSTEP_FORM };
    CNotes    m_notes;
    CButton    m_stepExecBox;
    CButton    m_stepSkippableBtn;
    CButton    m_stepSkipBtn;
    CButton    m_stepParamBox;
    CListCtrl    m_stepParamListCtrl;
    CButton    m_stepDetailsBox;
    CEdit    m_stepTypeEdit;
    CEdit    m_stepDescEdit;
    CEdit    m_stepNameEdit;
    CSpinButtonCtrl    m_secondSpin;
    CEdit    m_secondEdit;
    CSpinButtonCtrl    m_minuteSpin;
    CEdit    m_minuteEdit;
    // }}AFX_DATA

// Attributes
public:

protected:
    PV::Step *m_step;
    bool m_readOnly;
    bool m_customising;

// Operations
protected:
    unsigned long CalculateStepDelay();
    void SetStepDelay();
    void ResetMenusAndToolbars();

public:
    void PopulateData(PV::Step *step);
    void EnableEditing();
    void EnableCustomisation();

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanStepForm)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CPlanStepForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CPlanStepForm)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnChangeHourEdit();
    afx_msg void OnChangeMinuteEdit();
    afx_msg void OnChangeSecondEdit();
    afx_msg void OnChangeStepDescriptionEdit();
    afx_msg void OnChangeStepNameEdit();
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSetFocus(CWnd* pNewWnd);
    afx_msg void OnStepskipCheck();
    afx_msg void OnDblclkStepparamList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnStepskippableCheck();
    afx_msg void OnStepExit();
    afx_msg void OnStepEditparam();
    afx_msg void OnItemchangedStepparamList(NMHDR* pNMHDR, LRESULT* pResult);
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANSTEPFORM_H__9CBBF271_BC7C_11D5_B646_0050BAB1D92B__INCLUDED_)
