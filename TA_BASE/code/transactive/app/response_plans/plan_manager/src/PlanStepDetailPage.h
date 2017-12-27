//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepDetailPage.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANSTEPDETAILPAGE_H__CE627EA1_D57E_4D50_BDE8_97601C4B0D43__INCLUDED_)
#define AFX_PLANSTEPDETAILPAGE_H__CE627EA1_D57E_4D50_BDE8_97601C4B0D43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/editable_list_control/ICellManager.h"
#include "PlanDetailPage.h"
#include "PlanInstanceComboBox.h"
#include "SinglePlanInstanceListCtrl.h"
#include "PlanStepListCtrl.h"
#include "PlanStepParameterListCtrl.h"
#include "YSplitterWnd.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepDetailPage dialog

    class CPlanStepDetailPage : public CPlanDetailPage,
                                public ICellManager
    {
    // Construction
    public:
        CPlanStepDetailPage();   // standard constructor
        virtual ~CPlanStepDetailPage();

    // Operations
    public:
        // ICellManagerInterface
        CellType GetCellType(CEditableListCtrl* listCtrl, int item, int subItem);
        unsigned long GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem);
        CellData GetCellData(CEditableListCtrl* listCtrl, int item, int subItem);
        CString GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem);
        char GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem);
        bool UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value);

    protected:
        virtual void ShowRelevantControls();
        virtual void RefreshControlContents();
        virtual void RefreshActiveInstances();
        virtual void PurgeActiveInstances();


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepDetailPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanStepDetailPage)
        virtual BOOL OnInitDialog();
        virtual BOOL OnSetActive();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnPlanInstanceComboItemSelected();
        afx_msg void OnPlanStepsListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnDblclkPlanStepsList(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        afx_msg LRESULT OnSelectActivePlanInstance(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnActivePlanDetailUpdate(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnPlanCustomisationUpdate(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnRefreshPlanStepList(WPARAM wParam, LPARAM lParam);

        DECLARE_MESSAGE_MAP()


    // Dialog Data
    protected:
        // {{AFX_DATA(CPlanStepDetailPage)
        enum { IDD = IDD_PLAN_STEP_DETAIL_DLG };
        CStatic    m_planInstanceComboLabel;
        CSinglePlanInstanceListCtrl    m_planInstanceListCtrl;
        CPlanStepParameterListCtrl    m_stepParametersListCtrl;
        CButton    m_stepParametersGroupBox;
        CPlanStepListCtrl    m_planStepsListCtrl;
        CButton    m_planStepsGroupBox;
        CButton    m_planStatusGroupBox;
        CPlanInstanceComboBox    m_planInstanceCombo;
        // }}AFX_DATA
        CySplitterWnd m_splitterBar;

        friend class PlanNode;
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPDETAILPAGE_H__CE627EA1_D57E_4D50_BDE8_97601C4B0D43__INCLUDED_)
