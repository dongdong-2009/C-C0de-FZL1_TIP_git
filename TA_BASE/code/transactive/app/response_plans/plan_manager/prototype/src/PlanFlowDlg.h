/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanFlowDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANFLOWDLG_H__EBB84EFE_A446_4823_8DD9_7AAD7D01259B__INCLUDED_)
#define AFX_PLANFLOWDLG_H__EBB84EFE_A446_4823_8DD9_7AAD7D01259B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanFlowDlg.h : header file
//

#include "Notes.h"
#include "PictureEx.h"
#include "PvCommonDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CPlanFlowDlg dialog

class CPlanFlowDlg : public CDialog
{
// Construction
public:
    CPlanFlowDlg(CMenu *menu, CToolBar *editToolBar, CToolBar *ctrlToolBar);

// Dialog Data
    // {{AFX_DATA(CPlanFlowDlg)
    enum { IDD = IDD_PLANFLOW_DIALOG };
    CStatic    m_planOwnerLabel;
    CStatic    m_planInstLabel;
    CStatic    m_planExecStateLabel;
    CStatic    m_planExecModeLabel;
    CPictureEx    m_flowImage;
    CEdit    m_planInstEdit;
    CButton    m_planStatusBox;
    CButton    m_planFlowBox;
    CEdit    m_planOwnerEdit;
    CComboBox    m_planInstCombo;
    CEdit    m_planExecStateEdit;
    CEdit    m_planExecModeEdit;
    CScrollBar    m_flowVSBar;
    CScrollBar    m_flowHSBar;
    CNotes    m_notes;
    // }}AFX_DATA

// Attributes
protected:
    PV::Plan *m_plan;
    PV::PlanInstance *m_planInstance;
    bool m_readOnly;
    bool m_controlMode;
    CMenu *m_menu;
    CToolBar *m_editToolBar;
    CToolBar *m_ctrlToolBar;

// Operations
protected:
    void PopulateData();
    void ResetMenusAndToolbars();

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanFlowDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    // {{AFX_MSG(CPlanFlowDlg)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangePlaninstCombo();
    // }}AFX_MSG
    afx_msg LRESULT OnUpdatePlanDetails(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnableEditing(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnableControl(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANFLOWDLG_H__EBB84EFE_A446_4823_8DD9_7AAD7D01259B__INCLUDED_)
