/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanDetailsDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANDETAILSDLG_H__113C51A4_E53C_4997_95FA_0A29D87C713C__INCLUDED_)
#define AFX_PLANDETAILSDLG_H__113C51A4_E53C_4997_95FA_0A29D87C713C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanDetailsDlg.h : header file
//

#include "YSplitterWnd.h"
#include "PvCommonDefs.h"
#include "Notes.h"


/////////////////////////////////////////////////////////////////////////////
// CPlanDetailsDlg dialog

class CPlanDetailsDlg : public CDialog
{
// Construction
public:
    CPlanDetailsDlg(CMenu *menu, CToolBar *editToolBar, CToolBar *ctrlToolBar);

// Dialog Data
    // {{AFX_DATA(CPlanDetailsDlg)
    enum { IDD = IDD_PLAN_DETAILS_DIALOG };
    CEdit    m_planAppStatusEdit;
    CNotes    m_notes;
    CEdit    m_planExecModeEdit;
    CEdit    m_planNameEdit;
    CEdit    m_planDescEdit;
    CButton    m_activePlansBox;
    CListCtrl    m_activePlansListCtrl;
    CButton    m_planDetailsBox;
    // }}AFX_DATA

// Attributes
protected:
    CMenu *m_menu;
    CToolBar *m_editToolBar;
    CToolBar *m_ctrlToolBar;
    PV::Plan *m_plan;
    bool m_readOnly;
    bool m_controlMode;

    CySplitterWnd m_splitter;
    double m_splitRatio; // vertical space above the splitter vs. that below the splitter

// Operations
protected:
    void ResetMenusAndToolbars();

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanDetailsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    // {{AFX_MSG(CPlanDetailsDlg)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkActiveplanList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemchangedActiveplanList(NMHDR* pNMHDR, LRESULT* pResult);
    // }}AFX_MSG
    afx_msg LRESULT OnUpdatePlanDetails(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnableEditing(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnableControl(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSplitterMoved(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANDETAILSDLG_H__113C51A4_E53C_4997_95FA_0A29D87C713C__INCLUDED_)
