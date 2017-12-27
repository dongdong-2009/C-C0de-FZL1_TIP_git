//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/InteractiveStepDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2015/01/20 12:26:46 $
// Last modified by:  $Author: CM $
//
// <description>

#pragma once

#pragma warning (disable : 4250)
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#pragma warning (default : 4250)

namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CInteractiveStepDlg dialog

    class CInteractiveStepDlg : public CDialog
    {
        DECLARE_DYNAMIC(CInteractiveStepDlg)
    // Construction
    protected:
        CInteractiveStepDlg(UINT nIDTemplate, const TA_Base_Core::CurrentPlanStepDetail& stepDetail, TA_Base_Core::Seconds timeout); // standard constructor
        virtual ~CInteractiveStepDlg();

    // Operations
    protected:

        virtual CString getTitlePrefix() const = 0;
        virtual void reportCompletionStatus() const = 0;
        virtual void startTimeoutCountdown();
        virtual void updateTimeoutCountdownDisplay();

        virtual void setCompletionStatus(int exitCode);
        virtual TA_Base_Core::EPlanStepCompletionStatus getCompletionStatus() const;
        virtual TA_Base_Core::CurrentPlanStepDetail getStepDetail() const;

        CString getLabel(UINT labelResourceID, const char *defaultLabel) const;

    // Attributes
    private:
        TA_Base_Core::CurrentPlanStepDetail m_stepDetail;
        TA_Base_Core::EPlanStepCompletionStatus m_completionStatus;
        unsigned long m_timeout;
        unsigned long m_secondsToTimeout;

        bool m_stepExecutionInterrupted;


    // Dialog Data

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CInteractiveStepDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        virtual void OnCancel();
        virtual void PostNcDestroy();
        // }}AFX_VIRTUAL

        // Generated message map functions
        // {{AFX_MSG(CInteractiveStepDlg)
        afx_msg void OnTimer(UINT nIDEvent);
        afx_msg void OnDestroy();
        // }}AFX_MSG
        afx_msg LRESULT OnStepExecutionInterrupted(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App
