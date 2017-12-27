//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanGeneralDetailPage.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_PLANGENERALDETAILPAGE_H__27738064_C8A4_4CDA_B629_C82925359FE4__INCLUDED_)
#define AFX_PLANGENERALDETAILPAGE_H__27738064_C8A4_4CDA_B629_C82925359FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanDetailPage.h"
#include "PlanInstanceListCtrl.h"
#include "YSplitterWnd.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanGeneralDetailPage dialog

    class CPlanGeneralDetailPage : public CPlanDetailPage
    {
    // Construction
    public:
        CPlanGeneralDetailPage();   // standard constructor
        virtual ~CPlanGeneralDetailPage();


    // Operations
    protected:
        virtual void ShowRelevantControls();
        virtual void RefreshControlContents();
        virtual void RefreshActiveInstances();
        virtual void PurgeActiveInstances();


    // Attributes
    private:
        bool m_isLoadingControls;

    // Overrides
    protected:
       // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanGeneralDetailPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanGeneralDetailPage)
        virtual BOOL OnInitDialog();
        virtual BOOL OnSetActive();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnPlanInstanceListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnChangeNameEdit();
        afx_msg void OnChangeDescriptionEdit();
        // }}AFX_MSG
        afx_msg LRESULT OnRefreshPlanGeneralDetail(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()


    // Dialog Data
    protected:
        // {{AFX_DATA(CPlanGeneralDetailPage)
        enum { IDD = IDD_PLAN_GENERAL_DETAIL_DLG };
        CEdit    m_planModByEdit;
        CEdit    m_planModTimeEdit;
        CEdit    m_approvalModByEdit;
        CEdit    m_approvalModTimeEdit;
        CEdit    m_nameEdit;
        CEdit    m_descriptionEdit;
        CEdit    m_approvalStatusEdit;
        CButton    m_activeInstancesGroupBox;
        CButton    m_detailsGroupBox;
        CPlanInstanceListCtrl    m_planInstanceListCtrl;
        // }}AFX_DATA
        CySplitterWnd m_splitterBar;

        friend class PlanNode;
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANGENERALDETAILPAGE_H__27738064_C8A4_4CDA_B629_C82925359FE4__INCLUDED_)
