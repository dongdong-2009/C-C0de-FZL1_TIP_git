//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/DecisionDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#pragma once

#include "app/response_plans/plan_mft_processor/src/resource.h"
#include "app/response_plans/plan_mft_processor/src/InteractiveStepDlg.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CDecisionDlg dialog

    class CDecisionDlg : public CInteractiveStepDlg
    {
        DECLARE_DYNAMIC(CDecisionDlg)
    // Construction/destruction
    public:
        CDecisionDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~CDecisionDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CDecisionDlg)
        enum { IDD = IDD_DECISION_DLG };
        CString    m_query;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CDecisionDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog(); //DP-changes
        // }}AFX_VIRTUAL

        // Generated message map functions
        // {{AFX_MSG(CDecisionDlg)
        afx_msg void OnYes();
        afx_msg void OnNo();
        afx_msg void OnDestroy();
        afx_msg void OnPause(); //DP-changes
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App
