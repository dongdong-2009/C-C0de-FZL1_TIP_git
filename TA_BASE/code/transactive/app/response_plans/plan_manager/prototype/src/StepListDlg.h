#if !defined(AFX_STEPLISTDLG_H__1568D053_E68E_41A0_A717_143BAB3D35A7__INCLUDED_)
#define AFX_STEPLISTDLG_H__1568D053_E68E_41A0_A717_143BAB3D35A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StepListDlg.h : header file
//

#include "bus/mfc_extensions/src/editable_list_control/ICellManager.h"
#include "bus/mfc_extensions/src/editable_list_control/EditableListCtrl.h"
#include "YSplitterWnd.h"
#include "PvCommonDefs.h"
#include "Notes.h"


/////////////////////////////////////////////////////////////////////////////
// CStepListDlg dialog

class CStepListDlg : public CDialog, ICellManager
{
// Construction
public:
    CStepListDlg(CMenu *menu, CToolBar *editToolBar, CToolBar *ctrlToolBar);

// Dialog Data
    // {{AFX_DATA(CStepListDlg)
    enum { IDD = IDD_STEPLIST_DIALOG };
    CStatic    m_planOwnerLabel;
    CStatic    m_planInstLabel;
    CStatic    m_planExecStateLabel;
    CStatic    m_planExecModeLabel;
    CButton    m_stepParamBox;
    CEditableListCtrl    m_stepParamListCtrl;
    CNotes    m_notes;
    CButton    m_planStatusBox;
    CButton    m_planStepsBox;
    CEditableListCtrl    m_planStepListCtrl;
    CEdit    m_planOwnerEdit;
    CComboBox    m_planInstCombo;
    CEdit    m_planExecStateEdit;
    CEdit    m_planExecModeEdit;
    // }}AFX_DATA

// Attributes
protected:
    CMenu *m_menu;
    CToolBar *m_editToolBar;
    CToolBar *m_ctrlToolBar;
    PV::Plan *m_plan;
    PV::PlanInstance *m_planInstance;
    CImageList m_stepListImages;
    bool m_readOnly;
    bool m_controlMode;

    CySplitterWnd m_splitter;
    double m_splitRatio; // vertical space above the splitter vs. that below the splitter

// Operations
protected:
    void PopulateData();
    void ResetMenusAndToolbars();

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CStepListDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    // {{AFX_MSG(CStepListDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSelchangePlaninstCombo();
    afx_msg void OnDblclkPlanstepList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemchangedPlanstepList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkStepparamList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnStepEdit();
    afx_msg void OnStepCustomise();
    // }}AFX_MSG
    afx_msg LRESULT OnUpdatePlanDetails(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnableEditing(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEnableControl(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSplitterMoved(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

// ICellManager
public:
    CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);
    CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);
    bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEPLISTDLG_H__1568D053_E68E_41A0_A717_143BAB3D35A7__INCLUDED_)
